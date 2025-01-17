/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 1.17 $ of : mfd-data-access.m2c,v $ 
 *
 * $Id$
 */
/*
 * standard Net-SNMP includes 
 */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

/*
 * include our parent header 
 */
#include "ipSystemStatsTable.h"


#include "ipSystemStatsTable_data_access.h"

static int      ipss_cache_refresh = IPSYSTEMSTATSTABLE_CACHE_TIMEOUT;

/** @ingroup interface 
 * @addtogroup data_access data_access: Routines to access data
 *
 * These routines are used to locate the data used to satisfy
 * requests.
 * 
 * @{
 */
/**********************************************************************
 **********************************************************************
 ***
 *** Table ipSystemStatsTable
 ***
 **********************************************************************
 **********************************************************************/
/*
 * IP-MIB::ipSystemStatsTable is subid 1 of ipTrafficStats.
 * Its status is Current.
 * OID: .1.3.6.1.2.1.4.31.1, length: 9
 */

/**
 * initialization for ipSystemStatsTable data access
 *
 * This function is called during startup to allow you to
 * allocate any resources you need for the data table.
 *
 * @param ipSystemStatsTable_reg
 *        Pointer to ipSystemStatsTable_registration
 *
 * @retval MFD_SUCCESS : success.
 * @retval MFD_ERROR   : unrecoverable error.
 */
int
ipSystemStatsTable_init_data(ipSystemStatsTable_registration *
                             ipSystemStatsTable_reg)
{
   DEBUGMSGTL(("verbose:ipSystemStatsTable:ipSystemStatsTable_init_data",
                "called\n"));

    /*
     * TODO:303:o: Initialize ipSystemStatsTable data.
     */

    netsnmp_access_systemstats_init();
    return MFD_SUCCESS;
}                               /* ipSystemStatsTable_init_data */

/**
 * container overview
 *
 */

/**
 * container initialization
 *
 * @param container_ptr_ptr A pointer to a container pointer. If you
 *        create a custom container, use this parameter to return it
 *        to the MFD helper. If set to NULL, the MFD helper will
 *        allocate a container for you.
 * @param  cache A pointer to a cache structure. You can set the timeout
 *         and other cache flags using this pointer.
 *
 *  This function is called at startup to allow you to customize certain
 *  aspects of the access method. For the most part, it is for advanced
 *  users. The default code should suffice for most cases. If no custom
 *  container is allocated, the MFD code will create one for your.
 *
 *  This is also the place to set up cache behavior. The default, to
 *  simply set the cache timeout, will work well with the default
 *  container. If you are using a custom container, you may want to
 *  look at the cache helper documentation to see if there are any
 *  flags you want to set.
 *
 * @remark
 *  This would also be a good place to do any initialization needed
 *  for you data source. For example, opening a connection to another
 *  process that will supply the data, opening a database, etc.
 */
void
ipSystemStatsTable_container_init(netsnmp_container **container_ptr_ptr,
                                  netsnmp_cache * cache)
{
    DEBUGMSGTL(("verbose:ipSystemStatsTable:ipSystemStatsTable_container_init", "called\n"));

    if (NULL == container_ptr_ptr) {
        snmp_log(LOG_ERR,
                 "bad container param to ipSystemStatsTable_container_init\n");
        return;
    }

    /*
     * For advanced users, you can use a custom container. If you
     * do not create one, one will be created for you.
     */


    *container_ptr_ptr = netsnmp_container_find("ipSystemStatsTable:table_container");
    if (NULL != *container_ptr_ptr)
        (*container_ptr_ptr)->container_name = strdup("ipSystemStatsTable");

    if (NULL == cache) {
        snmp_log(LOG_ERR,
                 "bad cache param to ipSystemStatsTable_container_init\n");
        return;
    }

    /*
     * TODO:345:A: Set up ipSystemStatsTable cache properties.
     *
     * Also for advanced users, you can set parameters for the
     * cache. Do not change the magic pointer, as it is used
     * by the MFD helper. To completely disable caching, set
     * cache->enabled to 0.
     */
    cache->timeout = IPSYSTEMSTATSTABLE_CACHE_TIMEOUT;  /* seconds */

    /*
     * don't release resources
     */
    cache->flags |=
        (NETSNMP_CACHE_DONT_AUTO_RELEASE | NETSNMP_CACHE_DONT_FREE_EXPIRED
         | NETSNMP_CACHE_DONT_FREE_BEFORE_LOAD |
         NETSNMP_CACHE_AUTO_RELOAD);
    ipSystemStatsTable_container_load(*container_ptr_ptr);
}                               /* ipSystemStatsTable_container_init */

/**
 * check entry for update
 */
static void
_check_for_updates(ipSystemStatsTable_rowreq_ctx * rowreq_ctx,
                   netsnmp_container *stats)
{
    /*
     * check for matching entry. works because indexes are the same.
     */
    netsnmp_systemstats_entry *systemstats_entry = (netsnmp_systemstats_entry*)
        CONTAINER_FIND(stats, rowreq_ctx->data);
    if (NULL == systemstats_entry) {
        DEBUGMSGTL(("ipSystemStatsTable:access",
                    "updating missing entry\n"));

        /*
         * mark row as missing, so we can set discontinuity
         * when it comes back.
         *
         * what else should we do? set refresh to 0? that's not quite right...
         */
        rowreq_ctx->known_missing = 1;
    } else {
        DEBUGMSGTL(("ipSystemStatsTable:access",
                    "updating existing entry\n"));

        /*
         * Check for changes & update
         */
        netsnmp_access_systemstats_entry_update(rowreq_ctx->data,
                                                systemstats_entry);

        /*
         * set discontinuity if previously missing.
         */
        if (1 == rowreq_ctx->known_missing) {
            rowreq_ctx->known_missing = 0;
            rowreq_ctx->ipSystemStatsDiscontinuityTime =
                netsnmp_get_agent_uptime();
        }

        /*
         * remove entry from container
         */
        CONTAINER_REMOVE(stats, systemstats_entry);
        netsnmp_access_systemstats_entry_free(systemstats_entry);
    }
}

/**
 * add new entry
 */
static void
_add_new(netsnmp_systemstats_entry *systemstats_entry,
         netsnmp_container *container)
{
    ipSystemStatsTable_rowreq_ctx *rowreq_ctx;

    DEBUGMSGTL(("ipSystemStatsTable:access", "creating new entry\n"));

    netsnmp_assert(NULL != systemstats_entry);
    netsnmp_assert(NULL != container);

    /*
     * allocate an row context and set the index(es)
     */
    rowreq_ctx =
        ipSystemStatsTable_allocate_rowreq_ctx(systemstats_entry, NULL);
    if ((NULL != rowreq_ctx)
        && (MFD_SUCCESS ==
            ipSystemStatsTable_indexes_set(rowreq_ctx,
                                           systemstats_entry->
                                           index[0]))) {
        rowreq_ctx->ipSystemStatsRefreshRate = ipss_cache_refresh * 1000;       /* milli-seconds */
        CONTAINER_INSERT(container, rowreq_ctx);
    } else {
        if (NULL != rowreq_ctx) {
            snmp_log(LOG_ERR, "error setting index while loading "
                     "ipSystemStatsTable cache.\n");
            ipSystemStatsTable_release_rowreq_ctx(rowreq_ctx);
        } else {
            snmp_log(LOG_ERR, "memory allocation failed while loading "
                     "ipSystemStatsTable cache.\n");
            netsnmp_access_systemstats_entry_free(systemstats_entry);
        }
    }
}

/**
 * container shutdown
 *
 * @param container_ptr A pointer to the container.
 *
 *  This function is called at shutdown to allow you to customize certain
 *  aspects of the access method. For the most part, it is for advanced
 *  users. The default code should suffice for most cases.
 *
 *  This function is called before ipSystemStatsTable_container_free().
 *
 * @remark
 *  This would also be a good place to do any cleanup needed
 *  for you data source. For example, closing a connection to another
 *  process that supplied the data, closing a database, etc.
 */
void
ipSystemStatsTable_container_shutdown(netsnmp_container *container_ptr)
{
    DEBUGMSGTL(("verbose:ipSystemStatsTable:ipSystemStatsTable_container_shutdown", "called\n"));

    if (NULL == container_ptr) {
        snmp_log(LOG_ERR,
                 "bad params to ipSystemStatsTable_container_shutdown\n");
        return;
    }

}                               /* ipSystemStatsTable_container_shutdown */

/**
 * load initial data
 *
 * TODO:350:M: Implement ipSystemStatsTable data load
 * This function will also be called by the cache helper to load
 * the container again (after the container free function has been
 * called to free the previous contents).
 *
 * @param container container to which items should be inserted
 *
 * @retval MFD_SUCCESS              : success.
 * @retval MFD_RESOURCE_UNAVAILABLE : Can't access data source
 * @retval MFD_ERROR                : other error.
 *
 *  This function is called to load the index(es) (and data, optionally)
 *  for the every row in the data set.
 *
 * @remark
 *  While loading the data, the only important thing is the indexes.
 *  If access to your data is cheap/fast (e.g. you have a pointer to a
 *  structure in memory), it would make sense to update the data here.
 *  If, however, the accessing the data involves more work (e.g. parsing
 *  some other existing data, or performing calculations to derive the data),
 *  then you can limit yourself to setting the indexes and saving any
 *  information you will need later. Then use the saved information in
 *  ipSystemStatsTable_row_prep() for populating data.
 *
 * @note
 *  If you need consistency between rows (like you want statistics
 *  for each row to be from the same time frame), you should set all
 *  data here.
 *
 */
int
ipSystemStatsTable_container_load(netsnmp_container *container)
{
    netsnmp_container *stats =
        netsnmp_access_systemstats_container_load(NULL, 0);


    DEBUGMSGTL(("verbose:ipSystemStatsTable:ipSystemStatsTable_cache_load",
                "called\n"));

    netsnmp_assert(NULL != container);

    if (NULL == stats)
        return MFD_RESOURCE_UNAVAILABLE;        /* msg already logged */
    /*
     * TODO:351:M: |-> Load/update data in the ipSystemStatsTable container.
     * loop over your ipSystemStatsTable data, allocate a rowreq context,
     * set the index(es) [and data, optionally] and insert into
     * the container.
     */
    /*
     * we just got a fresh copy of data. compare it to
     * what we've already got, and make any adjustements...
     */
    CONTAINER_FOR_EACH(container, (netsnmp_container_obj_func *)
                       _check_for_updates, stats);

    /*
     * now add any new entries
     */
    CONTAINER_FOR_EACH(stats, (netsnmp_container_obj_func *)
                       _add_new, container);

    /*
     * free the container. we've either claimed each ifentry, or released it,
     * so the dal function doesn't need to clear the container.
     */
    netsnmp_access_systemstats_container_free(stats,
                                              NETSNMP_ACCESS_SYSTEMSTATS_FREE_DONT_CLEAR);

    DEBUGMSGT(("verbose:ipSystemStatsTable:ipSystemStatsTable_cache_load",
               "%" NETSNMP_PRIz "u records\n", CONTAINER_SIZE(container)));

    return MFD_SUCCESS;
}                               /* ipSystemStatsTable_container_load */

/**
 * container clean up
 *
 * @param container container with all current items
 *
 *  This optional callback is called prior to all
 *  item's being removed from the container. If you
 *  need to do any processing before that, do it here.
 *
 * @note
 *  The MFD helper will take care of releasing all the row contexts.
 *
 */
void
ipSystemStatsTable_container_free(netsnmp_container *container)
{
    DEBUGMSGTL(("verbose:ipSystemStatsTable:ipSystemStatsTable_container_free", "called\n"));

    /*
     * TODO:380:M: Free ipSystemStatsTable container data.
     */
}                               /* ipSystemStatsTable_container_free */

/**
 * prepare row for processing.
 *
 *  When the agent has located the row for a request, this function is
 *  called to prepare the row for processing. If you fully populated
 *  the data context during the index setup phase, you may not need to
 *  do anything.
 *
 * @param rowreq_ctx pointer to a context.
 *
 * @retval MFD_SUCCESS     : success.
 * @retval MFD_ERROR       : other error.
 */
int
ipSystemStatsTable_row_prep(ipSystemStatsTable_rowreq_ctx * rowreq_ctx)
{
    DEBUGMSGTL(("verbose:ipSystemStatsTable:ipSystemStatsTable_row_prep",
                "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    /*
     * TODO:390:o: Prepare row for request.
     * If populating row data was delayed, this is the place to
     * fill in the row for this request.
     */

    return MFD_SUCCESS;
}                               /* ipSystemStatsTable_row_prep */

/** @} */
