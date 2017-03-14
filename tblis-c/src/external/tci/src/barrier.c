#include "tci.h"

#include <stdlib.h>

#if TCI_USE_PTHREAD_BARRIER

int tci_barrier_node_init(tci_barrier_node_t* barrier,
                          tci_barrier_node_t* parent,
                          unsigned nchildren)
{
    barrier->parent = parent;
    return pthread_barrier_init(&barrier->barrier, NULL, nchildren);
}

int tci_barrier_node_destroy(tci_barrier_node_t* barrier)
{
    return pthread_barrier_destroy(&barrier->barrier);
}

int tci_barrier_node_wait(tci_barrier_node_t* barrier)
{
    int ret = pthread_barrier_wait(&barrier->barrier);
    if (ret != 0 && ret != PTHREAD_BARRIER_SERIAL_THREAD) return ret;
    if (barrier->parent)
    {
        if (ret == PTHREAD_BARRIER_SERIAL_THREAD)
            tci_barrier_node_wait(barrier->parent);

        ret = pthread_barrier_wait(&barrier->barrier);
        if (ret != 0 && ret != PTHREAD_BARRIER_SERIAL_THREAD) return ret;
    }
    return 0;
}

#elif TCI_USE_SPIN_BARRIER

int tci_barrier_node_init(tci_barrier_node_t* barrier,
                          tci_barrier_node_t* parent,
                          unsigned nchildren)
{
    barrier->parent = parent;
    barrier->nchildren = nchildren;
    barrier->step = 0;
    barrier->nwaiting = 0;
    return 0;
}

int tci_barrier_node_destroy(tci_barrier_node_t* barrier)
{
    (void)barrier;
    return 0;
}

int tci_barrier_node_wait(tci_barrier_node_t* barrier)
{
    const unsigned old_step = __sync_fetch_and_add(&barrier->step, 0);

    if (__sync_add_and_fetch(&barrier->nwaiting, 1) == barrier->nchildren)
    {
        if (barrier->parent) tci_barrier_node_wait(barrier->parent);
        barrier->nwaiting = 0;
        __sync_fetch_and_add(&barrier->step, 1);
    }
    else
    {
        while (__sync_fetch_and_add(&barrier->step, 0) == old_step) tci_yield();
    }

    return 0;
}

#endif

int tci_barrier_is_tree(tci_barrier_t* barrier)
{
    return barrier->group_size > 1 &&
           barrier->group_size < barrier->nthread;
}

int tci_barrier_init(tci_barrier_t* barrier,
                     unsigned nthread, unsigned group_size)
{
    barrier->nthread = nthread;
    barrier->group_size = group_size;

    if (!tci_barrier_is_tree(barrier))
    {
        return tci_barrier_node_init(&barrier->barrier.single, NULL, nthread);
    }

    unsigned nbarrier = 0;
    unsigned nleaders = nthread;
    do
    {
        nleaders = (nleaders+group_size-1)/group_size;
        nbarrier += nleaders;
    }
    while (nleaders > 1);

    barrier->barrier.array =
        (tci_barrier_node_t*)malloc(sizeof(tci_barrier_node_t)*nbarrier);
    if (!barrier->barrier.array) return ENOMEM;

    unsigned idx = 0;
    unsigned nchildren = nthread;
    do
    {
        unsigned nparents = (nchildren+group_size-1)/group_size;
        for (unsigned i = 0;i < nparents;i++)
        {
            tci_barrier_node_t* node = barrier->barrier.array+idx+i;
            tci_barrier_node_t* parent =
                barrier->barrier.array+idx+nparents+i/group_size;
            unsigned nthreads_sub = TCI_MIN(group_size, nchildren-i*group_size);

            int ret = tci_barrier_node_init(node, parent, nthreads_sub);
            if (ret != 0)
            {
                for (unsigned j = idx+i;j --> 0;)
                {
                    tci_barrier_node_destroy(barrier->barrier.array+j);
                }
                free(barrier->barrier.array);
                return ret;
            }
        }
        idx += nparents;
        nchildren = nparents;
    }
    while (nchildren > 1);

    return 0;
}

int tci_barrier_destroy(tci_barrier_t* barrier)
{
    if (tci_barrier_is_tree(barrier))
    {
        unsigned nbarrier = 0;
        unsigned nleaders = barrier->nthread;
        unsigned group_size = barrier->group_size;
        do
        {
            nleaders = (nleaders+group_size-1)/group_size;
            nbarrier += nleaders;
        }
        while (nleaders > 1);

        for (unsigned i = 0;i < nbarrier;i++)
        {
            tci_barrier_node_destroy(barrier->barrier.array+i);
        }
        free(barrier->barrier.array);
    }
    else
    {
        return tci_barrier_node_destroy(&barrier->barrier.single);
    }

    return 0;
}

int tci_barrier_wait(tci_barrier_t* barrier, unsigned tid)
{
    if (tci_barrier_is_tree(barrier))
    {
         return tci_barrier_node_wait(barrier->barrier.array +
                                      tid/barrier->group_size);
    }
    else
    {
        return tci_barrier_node_wait(&barrier->barrier.single);
    }
}
