/**
 * Kostra programu pro 3. projekt IZP 2017/18
 *
 * Jednoducha shlukova analyza
 * Unweighted pair-group average
 * https://is.muni.cz/th/172767/fi_b/5739129/web/web/usrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    if (c != NULL){
      c->capacity = cap;
      c->size = 0;
      c->obj = malloc(sizeof(struct obj_t) * cap);
      if (c->obj == NULL){
	c->capacity = 0;

      }
    }
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);
    c->obj = NULL;
    c->size = 0;
    c->capacity = 0;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if (c->size >= c->capacity){
      int new_cap = c->capacity + CLUSTER_CHUNK;
      resize_cluster(c, new_cap);
    }
    c->obj[c->size] = obj;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    int prev_size = c1->size;

    for (int i = 0; i < c2->size; i++) {
	append_cluster(c1, c2->obj[i]);
    }
    if (c2->size > 0 && c1->size == prev_size + c2->size) {
	sort_cluster(c1);
    }
}
/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    int size_after_removal = narr - 1;
    clear_cluster(&carr[idx]);

    for (int i = idx; i < (narr-1); i++){
        carr[i] = carr[i+1];
    }
    return size_after_removal;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    float a = (o1->x - o2->x) * (o1->x - o2->x);
    float b = (o1->y - o2->y) * (o1->y - o2->y);

    float result = sqrtf(a + b);
    return result;
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float sum = 0;
    float count = 0;
    for (int i = 0; i < c1->size; i++){
        for (int j = 0; j < c2->size; j++){
          count++;
          sum += obj_distance(&c1->obj[i], &c2->obj[j]);
        }
    }
    return sum / count;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/

void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float distance = 0;
    float max_distance = cluster_distance(&carr[0], &carr[1]);
    *c1 = 0;
    *c2 = 1;
    for (int i = 0; i < narr; i++){
      for (int y = i+1; y < narr; y++){
        distance = cluster_distance(&carr[i], &carr[y]);
        if (max_distance > distance){
          *c1 = i;
          *c2 = y;
          max_distance = distance;
        }
      }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE

    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
  assert(arr != NULL);

  FILE *text_file;
  if ((text_file = fopen(filename, "r")) == NULL){
	fprintf(stderr, "Spatne zadany soubor \n");
	return -1;
  }

  int count = 0;
  struct obj_t obj;

  if ((fscanf(text_file, "count=%d", &count) != 1)){
	fprintf(stderr, "Nepovedlo se nacist prvni radek \n");
	fclose(text_file);
	return -1;
  }
  *arr = malloc(count * sizeof(struct cluster_t));

  if (*arr == NULL){
	fprintf(stderr, "Nepovedlo se alokovat \n");
        fclose(text_file);
	free(*arr);
	return -1;
  }

  for (int i = 0; i < count; i++){
   	if ((fscanf(text_file, "%d %f %f", &obj.id, &obj.x, &obj.y)) != 3){
		fprintf(stderr, "Nepodarili se naplnit objekty s X Y a ID \n");
		fclose(text_file);
		for (int j = 0; j < i; j++){
			clear_cluster(&(*arr)[j]);
		}
		return -1;
   	}
	if (text_file == NULL){
		fprintf(stderr, "error \n");
	}
	if (obj.x < 0 || obj.x > 1000 || obj.y < 0 || obj.y > 1000 ){
		fprintf(stderr, "Spatne hodnoty v X Y nebo ID \n");
		fclose(text_file);
		return -1;
	}
	init_cluster(&(*arr)[i], 1);
	append_cluster(&(*arr)[i], obj);

  }

  fclose(text_file);
  return count;
}


/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
  struct cluster_t *clusters;

  int pos_a, pos_b;
  int count;
  int x_clusters;

  if (argc == 1){
	fprintf(stderr, "Musi byt uvedena cesta k souboru \n");
	return -1;
  }
  else if (argc == 2){
	x_clusters = 1;
  }
  else if (argc == 3){
    	x_clusters = atoi(argv[2]);
  }
  else{
	fprintf(stderr, "Spatne argumenty \n");
	return -1;
  }

  count = load_clusters(argv[1], &clusters);

  if (count <= 0){
	fprintf(stderr, "Pocet clusteru je < 0 \n");
	return -1;
  }
  if (x_clusters > count || x_clusters <= 0){
  	fprintf(stderr, "Max pocet shluku je pocet objektu  a min je 1\n");
	for (int i = 0; i < count; i++){
		clear_cluster(&clusters[i]);
	}
	return -1;
  }

  while (count != x_clusters){
       	find_neighbours(clusters, count, &pos_a, &pos_b);
 	merge_clusters(&clusters[pos_a], &clusters[pos_b]);
       	count = remove_cluster(clusters, count, pos_b);
  }
  print_clusters(clusters, count);
  for (int i = 0; i < count; i++){
  	clear_cluster(&clusters[i]);
  }
  free(clusters);


  return 0;

}
