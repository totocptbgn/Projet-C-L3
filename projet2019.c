#include "projet2019.h"

static size_t nb_blocs(size_t o)
{
	if (o % sizeof(align_data) == 0) {
		return o / sizeof(align_data);
	}
	return o / sizeof(align_data) + 1;
}

void* ld_create(size_t nboctets)
{
	align_data* mem = malloc(nb_blocs(nboctets) * sizeof(align_data));
	tranche* tab_tranches = calloc(nb_blocs(nboctets), sizeof(tranche));
	tab_tranches[0].decalage = 0;
	tab_tranches[0].nb_blocs = nb_blocs(nboctets);
	head* hd = malloc(sizeof(head));
	hd -> first = NULL;
	hd -> last = NULL;
	hd -> libre = tab_tranches;
	hd -> memory = mem;
	hd -> size = nb_blocs(nboctets);
	return hd;
}

/**
 * Affiche tout les éléments de la liste, en affichant leurs données sous forme d'int,
 * puis affiche la longueur de la liste.
 */

void print_list_int(void* liste, int rev)
{
	if(liste == NULL) return;
	head* hd = liste;
	int index = 0;

	if(!rev)
	{
		node* n = hd -> first;
		if (n == NULL) {
			printf(" - Length of the list : %d\n", index);
			printf(" - Free space = %zu\n\n", ld_total_free_memory(liste));
			return;
		}
		do
		{
			printf("[ %d | %p ]", *((int*) n -> data),n);
			if (ld_next(liste, n) != NULL) printf(" - ");
			index++;
			if (n -> next == 0) break;
			n = ld_next(liste, n);
		}
		while (n != NULL);
	} else
	{
		node* n = hd -> last;
		if (n == NULL) {
			printf(" - Length of the list : %d\n", index);
			printf(" - Free space = %zu bytes.\n\n", ld_total_free_memory(liste));
			return;
		}
		do
		{
			printf("[ %d | %p ]", *((int*) n -> data),n);
			if (ld_previous(liste, n) != NULL) printf(" - ");
			index++;
			if (n -> previous == 0) break;
			n = ld_previous(liste, n);
		}
		while (n != NULL);
	}
	printf("\n\n");
	printf(" - Length of the list : %d\n", index);
	printf(" - Free space = %zu\n\n", ld_total_free_memory(liste));
}

/**
 * Revoie le i ème élément de la liste.
 * Renvoie NULL si l'élément n'est pas trouvé.
 */


void* ld_get_at(void* liste, int i)
{
	// printf("call ld_get_at(liste, %d)\n", i);
	node* n = ld_first(liste);
	if (n == NULL) return NULL;
	for (size_t j = 0; j < i; j++)
	{
		n = ld_next(liste, n);
		if (n == NULL) return NULL;
	}
	// printf("-> return %p;\n", n);
	return n;
}

void* ld_first(void* liste)
{
	if (liste == NULL) return NULL;
	return ((head*)liste) -> first;
}

void* ld_last(void* liste)
{
	if (liste == NULL) return NULL;
	return ((head*)liste) -> last;
}

void* ld_next(void* liste, void* current)
{
	if (liste == NULL || current == NULL) return NULL;
	node* n = current;
	if (liste == current) return ld_first(liste);
	if (n == NULL) return NULL;
	if (n -> next == 0) return NULL;
	return (align_data*) n + n -> next;
}

void* ld_previous(void* liste, void* current)
{
	if (liste == NULL || current == NULL) return NULL;
	node* n = current;
	if (liste == current) {
		return ld_first(liste);
	}
	if (n == NULL) return NULL;
	if (n -> previous==0) return NULL;
	return (align_data*) n + n -> previous;
}

void ld_destroy (void* liste)
{
	head* hd = liste;
	if(liste != NULL) {
		void* mem = hd -> memory;
		tranche* trch = hd -> libre;
		free(mem);
		free(trch);
		free(hd);
	}
}

size_t ld_get(void* liste, void* current, size_t len, void* val)
{
	if(liste == NULL || current == NULL) return 0;
	node* n = current;
	if (len > n -> len)
	{
		len = n -> len;
	}
	char* tabval = val;
	for (size_t i = 0; i < len; i++)
	{
		tabval[i] = ((char*) n -> data)[i];
	}
	return len;
}

static void deleteTranche(void* liste, void* current)
{
	head* hd = liste;
	tranche* tranches = hd -> libre;
	tranche* el = current;
	while (el < tranches + hd -> size - 1 && el -> nb_blocs != 0)
	{
		el -> decalage = (el+1) -> decalage;
		el -> nb_blocs = (el+1) -> nb_blocs;
		el = el + 1;
	}
	el -> decalage = 0;
	el -> nb_blocs = 0;
}

static void* ld_create_node(size_t len, void* p_data, void* liste)
{
	size_t taille = nb_blocs(len+sizeof(node));
	head* hd = liste;
	tranche* first = hd -> libre;
	tranche* current = hd -> libre;
	while (current+1 < first + hd -> size && (current + 1) -> nb_blocs != 0 && (current -> nb_blocs) < taille )
	{	
		current++;
	}
	if(current >= (first) + hd -> size || (current) -> nb_blocs == 0 || (current -> nb_blocs) < taille) {
		return NULL; // Sort du tableau ou pas assez de place
	}
	node* n = (hd -> memory) + (current -> decalage) * sizeof(align_data);
	if (current -> nb_blocs > taille)
	{
		current -> nb_blocs -= taille;
		current -> decalage += taille;
	} else {
		deleteTranche(liste, current);
	}
	*(n -> data) = *((align_data*)p_data);
	n -> next = 0;
	n -> previous = 0;
	n -> len = taille;
	return n;
}

/**
 * Renvoie la distance de B à A
 */

static long int dist(void* a, void* b)
{
	return ((align_data*) a) - ((align_data*) b);
}

void* ld_insert_first (void* liste, size_t len, void* p_data)
{
	if( liste == NULL) return NULL;
	head* hd = liste;
	node* new = ld_create_node(len, p_data, liste);
	if (new == NULL) return NULL;
	if (ld_first(hd) != NULL)
	{
		((node*) ld_first(liste)) -> previous = dist(new, ld_first(liste));
		new -> next = dist(ld_first(liste), new);
	}
	else
	{
		hd -> last = new;
	}
	hd -> first = new;
	return new;
}

void* ld_insert_last (void* liste, size_t len, void* p_data)
{
	if(liste == NULL) return NULL;
	head* hd = liste;
	node* new = ld_create_node(len, p_data, liste);
	if(new == NULL) return NULL;
	if (ld_last(hd) != NULL)
	{
		((node*) ld_last(liste)) -> next = dist(new, ld_last(liste));
		new -> previous = dist(ld_last(liste), new);
	} else
	{
		hd -> first = new;
	}
	hd -> last = new;
	return new;
}

void* ld_insert_before (void* liste, void* n, size_t len, void* p_data)
{
	if (n == NULL || liste == NULL) return NULL;
	if (ld_first(liste) == NULL || ld_first(liste) == n) return ld_insert_first(liste, len, p_data);
	node* new = ld_create_node(len, p_data, liste);
	if(new == NULL) return NULL;
	node* prev = ld_previous(liste, n);
	node* next = n;
	prev -> next = dist(new, prev);
	new -> previous = dist(prev, new);
	new -> next = dist(next, new);
	next -> previous = dist(new, next);
	return new;
}

void* ld_insert_after (void* liste, void* n, size_t len, void* p_data)
{
	if (n == NULL || liste==NULL) return NULL;
	if (ld_first(liste) == NULL || ld_last(liste) == n) return ld_insert_last(liste, len, p_data);
	node* new = ld_create_node(len, p_data, liste);
	if(new==NULL) return NULL;
	node* prev = n;
	node* next = ld_next(liste, n);
	prev -> next = dist(new, prev);
	new -> previous = dist(prev, new);
	new -> next = dist(next, new);
	next -> previous = dist(new, next);
	return new;
}

/*
	Fusionne les tranches libres du tableau
*/
static void fusion(void* liste) {
	head* hd = liste;
	tranche* cur = hd -> libre;

	while(cur -> nb_blocs != 0) {
		//printf("%ld %ld %ld\n",(cur+1)->decalage,(cur+1)->nb_blocs,cur->decalage+cur->nb_blocs);
		if((cur + 1) -> nb_blocs != 0 && (cur + 1) -> decalage == cur -> decalage + cur -> nb_blocs)
		{
			cur -> nb_blocs += (cur + 1) -> nb_blocs;
			tranche* next = cur + 1;
			// Decalage du reste des tranches
			while(next < hd -> libre + hd -> size && next -> nb_blocs != 0)
			{
				next -> decalage = (next + 1) -> decalage;
				next -> nb_blocs = (next + 1) -> nb_blocs;
				next++;
			}
		}
		else
		{
			cur++;
		}
	}
}

static void* addTranche(void* liste, void* current)
{
	head* hd = liste;
	tranche* cur = hd->libre;
	node* n = current;
	tranche* res = NULL;
	ptrdiff_t decalage = dist(n, hd->memory);
	//printf("decalage : %ld\n",decalage);
	size_t nb_blocs = n -> len;
	// On recherche où on peut placer la tranche, pas besoin de test depassement de tableau, ça ne devrait jamais arriver.
	while(cur -> nb_blocs != 0 && cur->decalage<decalage)
	{
		cur++;
	}
	// Sauvegarde des anciennes valeurs
	size_t precblocs = cur -> nb_blocs;
	ptrdiff_t precdecal = cur -> decalage;
	//printf("%ld %ld\n",precblocs,precdecal);
	// Assignement des valeurs
	cur -> nb_blocs = nb_blocs;
	cur -> decalage = decalage;
	res = cur;
	cur++;
	// Décalage des cases suivantes
	while (precblocs != 0 && cur - 1 < hd -> libre + hd -> size)
	{
		size_t blocstemp = cur->nb_blocs;
		ptrdiff_t decaltemp = cur->decalage;
		cur->decalage = precdecal;
		cur->nb_blocs = precblocs;
		precblocs = blocstemp;
		precdecal = decaltemp;
		cur=cur+1;
	}
	//printf("available : %ld\n",ld_total_free_memory(liste));
	fusion(liste);
	return res;
}

void* ld_delete_node (void* liste, void* n)
{
	if(liste==NULL || n==NULL) {
		return 0;
	}
	head* hd = liste;
	if(ld_first(liste) == n || ld_last(liste) == n) {
		if (ld_first(liste) == n)
		{
			node* newFirst = ld_next(liste, n);
			hd -> first = newFirst;
			if(newFirst!=NULL)
				newFirst -> previous = 0;
		}
		if (ld_last(liste) == n)
		{
			node* newLast = ld_previous(liste, n);
			hd -> last = newLast;
			if(newLast != NULL)
				newLast -> next = 0;
		}
	}
	else {
		node* prev = ld_previous(liste, n);
		node* next = ld_next(liste, n);
		prev -> next = dist(next, prev);
		next -> previous = dist(prev, next);
	}
	addTranche(liste, n);
	return liste;
}

size_t ld_total_free_memory(void* liste)
{
	if(liste == NULL) {
		return 0;
	}
	tranche* trch = ((head*) liste) -> libre;
	if(trch == NULL) return 0;
	size_t s = 0;
	//printf("nb blocs:%ld\n",((head*) liste) -> size);
	while (trch -> nb_blocs != 0 && trch < ((head*) liste) -> libre + ((head*) liste) -> size)
	{
		
		s += trch -> nb_blocs;
		trch++;
	}
	return s * sizeof(align_data);
}

size_t ld_total_useful_memory (void* liste)
{
	if(liste == NULL) {
		return 0;
	}
	tranche* trch = ((head*) liste) -> libre;
	size_t s = 0;
	while (trch -> nb_blocs != 0 && trch < ((head*) liste) -> libre + ((head*) liste) -> size)
	{
		//printf("%ld -\n",trch -> nb_blocs);
		if((trch -> nb_blocs)*sizeof(align_data)>=sizeof(node)) {
			s += trch -> nb_blocs;
		}
		trch++;
	}
	return s * sizeof(align_data);
}

void* ld_add_memory(void* liste, size_t nboctets)
{
	if(liste == NULL || nboctets <= 0) {
		return liste;
	}
	head* hd = liste;
	ptrdiff_t f=dist(hd->first,hd->memory);
	ptrdiff_t l=dist(hd->last,hd->memory);
	
	//printf("%ld %ld",f,l);
	if( hd -> size != 0) {
		void* newLibre = realloc(hd -> libre, sizeof(tranche) * (nb_blocs(nboctets) + hd -> size));
		if (newLibre == NULL) return NULL;
		void* newMemory = realloc(hd -> memory, sizeof(align_data) * (nb_blocs(nboctets) + hd -> size));
		if (newMemory == NULL) return NULL;
		hd -> memory = newMemory;
		hd -> libre = newLibre;
	
		node* n = newMemory + (hd -> size)*sizeof(align_data);
		n -> len = nb_blocs(nboctets);
		hd -> size += nb_blocs(nboctets);
		if(hd -> first != NULL) {
			hd -> first = newMemory + f*sizeof(align_data);
			hd -> last = newMemory + l*sizeof(align_data);
		}
		addTranche(liste,n);
	}
	else {
		void* newLibre = calloc(nb_blocs(nboctets), sizeof(tranche));
		if (newLibre == NULL) return NULL;
		void* newMemory = calloc(nb_blocs(nboctets), sizeof(tranche));
		if (newMemory == NULL) return NULL;
		hd -> memory = newMemory;
		hd -> libre = newLibre;
		hd -> size = nb_blocs(nboctets);
		(hd -> libre) -> nb_blocs = hd -> size;
		(hd -> libre) -> decalage = 0;
	}
	
	return liste;
	
}

void* ld_compactify(void* liste)
{
	head* hd1 = liste;
	if(ld_first(liste) != NULL) {
		head* hd2 = ld_create(hd1->size * sizeof(align_data));
		if(hd2 == NULL) return NULL;
		node* n = ld_first(liste);
		while(n != NULL)
		{
			ld_insert_last(hd2, n->len * sizeof(align_data) - sizeof(node), n->data);
			ld_delete_node(hd1, n);
			n = ld_first(liste);
		}
		void* memorytemp = hd1 -> memory;
		tranche* libretemp = hd1 -> libre;
		hd1 -> libre = hd2 -> libre;
		hd1 -> memory = hd2 -> memory;
		hd1 -> last = hd2 -> last;
		hd1 -> first = hd2 -> first;
		hd2 -> libre = libretemp;
		hd2 -> memory = memorytemp;
		ld_destroy(hd2);
	}
	return liste;
}

int ld_number_of_free_blocs(void* liste) {
	if(liste == NULL) return 0;
	head* hd = liste;
	tranche* libre = hd -> libre;
	int number = 0;
	while(libre -> nb_blocs != 0 && libre < hd -> libre + hd -> size) {
		number++;
		libre+=1;
	}
	return number;
}
