/**
 * @mainpage Projekt 3
 * @file proj3.h
 * @version 1.0
 * @date 12/11/2017
 * @author Martin Macháček <xmacha73@stud.fit.vutbr.cz>
 */

/**
 * @defgroup structures  @{
 * @brief      Structure defning a dot on XY coordinates system, with an ID.
 */
struct obj_t {
    int id;
    float x;
    float y;
};
/**
 * @brief      Cluster of objekts which are connected together
 */
struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};
/** @} */


/**
 * @defgroup   operations with clusters @{
 * @brief      Inicialization of a cluster, memory allocation for object capacity.
 *             If the cluster of objects has NULL value, capacity is 0
 *
 * @param      c     Pointer to cluster of objects
 * @param      cap   Capacity of the cluster (max amount of objects)
 * @post       Memory for cluster will be allocated
 *
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 * @brief      Deletes all objects from selected cluster
 *
 * @param      c     Pointer to cluster to be cleared
 * @post       cluster will be empty and its memory will be cleared
 */
void clear_cluster(struct cluster_t *c);

/**
 * @brief      Chunk of cluster objects. Used for reallocation (e.g. more capacity)
 */
extern const int CLUSTER_CHUNK;

/**
 * @brief      Resizes cluster, changes capacity of the cluster by given amount
 *
 * @param      c        Cluster to be resized
 * @param      new_cap  New capacity of the cluster
 * @pre        capacity of cluster 'c' is greater or equal to zero
 * @post       cluster will be resized
 *
 * @return     Pointer to the cluster
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * @brief      Appends an object to the end a chosen cluster. If the object doesnt have a place to append to, cluster will be resized
 *
 * @param      c     Poitner to cluster, to which the object will append to
 * @param      obj   Object to be appended
 * @pre        number of objects inside the cluster is greater or equal to zero
 * @post       object will be added to the end of cluster
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * @brief      First cluster gets objects of the second cluster (they merge).
 *             If the first cluster doesnt have enough capacity, it resizes.
 *             Objects are sorted by their ID
 *
 * @param      c1    Pointer to first cluster
 * @param      c2    Pointer to second cluster
 * @pre        number of objects in the second cluster is more than zero.
 * @post       first cluster will get objects from second cluster and it will get sorted by their ID
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);

/**
 * @brief      Removes a cluster, calls function clear_cluster for complete removal.
 *
 * @param      carr  Pointer to array of all clusters
 * @param      narr  Size of the array (amount of clusters in the array)
 * @param      idx   Index of the cluster
 * @post       one cluster from array of cluster will be removed. New size of array will be narr-1.
 *
 * @return     New size of the array.
 * @}
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * @defgroup   operations with objects @{
 * @brief      Calculates distance between two objects (Euclid)
 *
 * @param      o1    Pointer to the first object
 * @param      o2    Pointer to the second object
 * @post       distance between two objects will be calculated
 * @return     The distance between o1 and o2
 */

float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * @brief      Calculates distance between 2 clusters
 *
 * @param      c1    Pointer to the first cluster
 * @param      c2    Pointer to the second cluster
 * @post       distance between clusters will be calculated
 * @pre        number of objects in both clusters is greater than zero.
 *
 * @return     Distance of the two clusters
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 * @defgroup   operations with clusters @{
 * @brief      Finds two clusters closest to each other
 *
 * @param      carr  Pointer to an array of all clusters
 * @param      narr  Size of the array (amount of clusters in the array)
 * @param      c1    Pointer to the first cluster
 * @param      c2    Pointer to the second cluster
 * @post       c1 and c2 will have indexes of found clusters
 * @}
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * @defgroup   operations with objects @{
 * @brief      Sorts objects inside a chosen cluster ascendingly by their ID.
 *
 * @param      c     Pointer to cluster to be sorted
 * @post       objects in the second cluster will be sorted ascendingly by their ID.
 * @}
 */
void sort_cluster(struct cluster_t *c);

/**
 * @defgroup   operations with clusters @{
 * @brief      Prints a cluster
 *
 * @param      c     Pointer to the cluster to be printed
 * @post       print the cluster tos standart input
 */
void print_cluster(struct cluster_t *c);

/**
 * @brief      Loads cluster with objects from a 'filename' file.
 *             For each object, 1 cluster is created and saves the cluster into array of clusters.
 *             Allocates memory for the array of clusters.
 *
 * @param      filename  File, which the objects are imported from
 * @param      arr       Pointer to pointer to array of all clusters
 * @pre        file must exist. The file must be formatted correctly.
 * @post       for every object in file there will be cluster created
 *
 * @return     Number of clusters in the array of clusters.
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 * @brief      Prints the array of clusters
 *
 * @param      carr  Pointer to the first cluster (first item of cluster array)
 * @param      narr  Number of clusters to be printed
 * @post       clusters will be printed on standart input
 * @}
 */
void print_clusters(struct cluster_t *carr, int narr);
