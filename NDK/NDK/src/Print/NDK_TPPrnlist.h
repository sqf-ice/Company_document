#ifndef PRNLIST_H_
#define PRNLIST_H_

#include <errno.h>

/* * 通用链表节点结构
 */
typedef struct {
	void * next;    /**< 指向下一个节点 */
	void * element; /**< 指向节点存放的数据项 */
}prnlist_node_t;

/**
 * 通用链表头节点结构
 */
typedef struct {
	int num_elt;		/**< 节点总数 */
	prnlist_node_t * node; /**< 指向第一个结点 */
}prn_list_t;

#endif
