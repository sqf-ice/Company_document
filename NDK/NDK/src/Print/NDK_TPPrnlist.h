#ifndef PRNLIST_H_
#define PRNLIST_H_

#include <errno.h>

/* * ͨ������ڵ�ṹ
 */
typedef struct {
	void * next;    /**< ָ����һ���ڵ� */
	void * element; /**< ָ��ڵ��ŵ������� */
}prnlist_node_t;

/**
 * ͨ������ͷ�ڵ�ṹ
 */
typedef struct {
	int num_elt;		/**< �ڵ����� */
	prnlist_node_t * node; /**< ָ���һ����� */
}prn_list_t;

#endif
