/**
 * @defgroup list 通用单向链表
 * @{
 */
/*
 * @file list.c
 * @brief 通用单向链表实现
 *
 * 链表使用头节点，保存链表中的结点个数，和指向第一个结点的指针。
 * 实现的链表操作包括：
 * - 1.prn_list_init     初始化链表头节点
 * - 2.prn_list_uninit   销毁链表头节点
 * - 3.prn_list_eol      判断指定位置是否位于链表结束后
 * - 4.prn_list_get      获取存放在链表中指定位置的结点的数据项
 * - 5.prn_list_remove   移除，销毁指定位置的结点，并返回存放的数据项
 * - 6.prn_list_add      将某个数据项添加到链表中的指定位置（将为其创建一个结点）
 * - 7.prn_list_find     查找符合某项条件的结点，并返回其存放的数据项
 * - 8.prn_list_get_pos  根据结点存放的数据项指针，返回结点所在位置
 * - 9.prn_list_for_each 使用指定回调函数遍历链表
 * - 10.prn_list_cleanup 清除这个链表的结点，并调用用户指定的回调函数
 *
 * @version 0.98
 * @author yemt@newlandcomputer.com
 * @date 2010-01-07
 */
//#include "foreprint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NDK_TPPrnBDF.h"
#include "NDK_TPPrnlist.h"

/**
 * 初始化链表头节点，使用calloc保证数据清0
 * @param li  返回动态分配到的头节点指针
 * @return
 * - -1 失败
 * - 0  成功
 */
int prn_list_init(prn_list_t ** li)
{
    *li = (prn_list_t *)calloc(1, sizeof (prn_list_t));
    if (*li == NULL) {
        return -1;
    }
    return 0;
}

/**
 * 销毁链表头节点
 * @param li  要销毁的头节点指针
 * @return
 * - 0  成功
 */
int prn_list_uninit(prn_list_t * li)
{
    free(li);
    return 0;
}

/**
 * 判断指定位置是否位于链表尾结点之后
 * @param li  链表头节点
 * @param i   指定位置
 * @return
 * - 0  否
 * - 1  是
 */
int prn_list_eol(prn_list_t * li, int i)
{
    if (li == NULL) return 1;
    if ((i >= 0) && (i < li->num_elt)) {
        return 0;
    }
    return 1; /* end of list */
}

prnlist_node_t * prn_list_get_node(prn_list_t * li, int pos)
{
    int i = 0;
    prnlist_node_t * ntmp;

    if ((li == NULL) || (pos < 0) || (pos >= li->num_elt)) {
        /* element does not exist */
        return NULL;
    }
    ntmp = li->node;
    while (pos > i) {
        i ++;
        ntmp = (prnlist_node_t *)ntmp->next;
    }
    return ntmp;
}
/**
 * 返回指定位置节点存放的数据项
 * @param li     链表头节点
 * @param pos    指定位置
 * @return void* 数据项指针，NULL表示无数据项，或空链表，或指定的位置不在链表内
 */
void * prn_list_get(prn_list_t * li, int pos)
{
    prnlist_node_t * ntmp = prn_list_get_node(li, pos);
    if (ntmp) {
        return ntmp->element;
    }
    return NULL;
}

/**
 * 删除指定位置节点，并返回存放的数据项
 * @param li     链表头节点
 * @param pos    指定位置
 * @return void* 数据项指针，NULL表示无数据项，或空链表，或指定的位置不在链表内
 */
void* prn_list_remove(prn_list_t * li, int pos)
{
    int i = 0;
    void * element;
    prnlist_node_t * ntmp,** tmp;

    if ((li == NULL) || (pos < 0) || (pos >= li->num_elt)) {
        /* element does not exist */
        return NULL;
    }
    tmp = &(li->node);
    while (pos > i) {
        i ++;
        tmp = (prnlist_node_t **)&((*tmp)->next);
    }
    ntmp = *tmp;
    *tmp = (prnlist_node_t *)((*tmp)->next);
    -- li->num_elt;
    element = ntmp->element;
    free(ntmp);
    return (element);
}
/**
 * 在指定位置创建节点，并保存用户数据项到改节点
 * @param li       链表头节点
 * @param element  指向用户数据项的指针
 * @param pos      指定位置
 * @return int 返回新的节点数
 */
int prn_list_add(prn_list_t * li, void * element, int pos)
{
    int i = 0;
    prnlist_node_t * ntmp,** tmp;

    if (li == NULL) return -1;

    if (pos < 0 || pos >= li->num_elt) {
        pos = li->num_elt;
    }
    tmp = &(li->node);
    while (pos > i) {
        i ++;
        tmp = (prnlist_node_t **)&((*tmp)->next);
    }
    ntmp = (prnlist_node_t *)malloc(sizeof (prnlist_node_t));
    ntmp->element = element;
    ntmp->next = *tmp;
    *tmp = ntmp;
    return (++ li->num_elt);
}

/**
 * 查找符合条件的数据项
 * @param li        链表头节点
 * @param cmp_func  用户提供的回调函数，用于判断是否符合条件（0是，1否）
 * @param data      回调函数的第一个参数，由用户指定，第二个参数则固定为节点指针
 * @return void* 返回查找到的数据项指针，NULL表示空链表或未找到
 */
void * prn_list_find(prn_list_t * li, int (*cmp_func) (void * data, void * node), void * data)
{
    prnlist_node_t * ntmp;

    if (li == NULL) {
        return NULL;
    }
    ntmp = li->node;
    while (ntmp) {
        if (cmp_func(data, ntmp) == 0) {
            break;
        }
        ntmp = ntmp->next;
    }
    return (ntmp == NULL ? NULL : ntmp->element);
}

/**
 * 返回存放指定数据项的节点的位置
 * @param li    链表头节点
 * @param node  指向用户指定的数据项
 * @return int  返回该数据项的位置，-1表示空链表，空数据项，或找不到
 */
int prn_list_get_pos(prn_list_t * li, void * node)
{
    int i = 0;
    prnlist_node_t * ntmp;
    if (li == NULL || node == NULL) return -1;
    ntmp = li->node;
    while (ntmp) {
        if (ntmp->element == node) {
            return i;
        }
        i ++;
        ntmp = ntmp->next;
    }
    return -1;
}

/**
 * 用指定回调函数遍历链表
 * @param li    链表头节点
 * @param call  回调函数指针
 * @param data  回调函数的第一个参数，用户指定。第二个参数是节点位置，第三个参数为节点存放的数据项指针
 * @return 0
 */
int prn_list_for_each(prn_list_t * li, void (*call)(void * data, void * element), void * data)
{
    int i = 0;
    prnlist_node_t * ntmp = NULL;

    if (li == NULL)
        return 0;

    ntmp = li->node;
    while (ntmp) {
        if (call) {
            call(data, ntmp->element);
        }
        i ++;
        ntmp = ntmp->next;
    }
    return 0;
}

int prn_list_for_each_breakoff(prn_list_t * li, int (*call)(void * data, void * element), void * data)
{
    int i = 0;
    prnlist_node_t * ntmp = NULL;

    if (li == NULL)
        return 0;
    ntmp = li->node;
    while (ntmp) {
        if (call) {
            if(call(data, ntmp->element)<0)
                return -1;
        }
        i ++;
        ntmp = ntmp->next;
    }
    return 0;
}

/**
 * 清空，并销毁链表的所有节点，并为每个结点调用用户指定回调函数
 * @param li        链表头节点
 * @param callback  回调函数指针
 * @param data      回调函数的第一个参数，用户指定。第二个参数为节点存放的数据项指针
 * @param freehead  是否释放头节点，非0表示释放
 * @return 0
 */
int prn_list_cleanup(prn_list_t * li, void (*callback)(void * data, void * element), void * data, int freehead)
{
    prnlist_node_t * ntmp, *tmp;
    if (li == NULL) return 0;
    ntmp = li->node;
    while (ntmp) {
        if (callback) {
            callback(data, ntmp->element);
        }
        tmp = ntmp;
        ntmp = ntmp->next;
        free(tmp);
    }
    li->node = NULL;
    li->num_elt = 0;
    if (freehead) {
        free(li);
    }
    return 0;
}

/** @} */

