#include "projet2019.h"

void printNodeState(void* liste, void* n, char nameNode[])
{
    if (n == NULL)
    {
        printf("\n%s is NULL.\n", nameNode);
        return;
    }
    printf("\n%s : %p\n", nameNode, n);
    printf("%s.previous : %p (%ld)\n", nameNode, n + (((node* )n) -> previous * sizeof(align_data)), ((node* )n) -> previous);
    printf("%s.next : %p (%ld)\n", nameNode, n + (((node* )n) -> next) * sizeof(align_data), ((node* )n) -> next);
}

void printListeState(void* liste)
{
    head* hd = liste;
    printf("\nliste.first : %p\n", hd -> first);
    printf("liste.last : %p\n", hd -> last);
}

static void addMemory(void* liste)
{
    size_t mem = (sizeof(int) + sizeof(node)) * 5;
    ld_add_memory(liste, mem);
    printf("> Adding %zu bytes", mem);
    printf("\t\t\t(Available space : %ld bytes.)\n", ld_total_free_memory(liste));
}

void randomTests(int n, size_t sizeT)
{
    // On créer la liste
    void* liste = ld_create(sizeof(align_data) * sizeT);
    int size = 0;

    // On utilise un seed (l'heure) pour générer des nombres ramdom
    time_t t;
    srand((unsigned) time(&t));
    printf("\n\n\t\t\t--- Random Tests ---\n");
    printf("\nNumber of operations : %d\n", n);
    printf("Allocated Space : %lu bytes.\n", sizeof(align_data) * sizeT);
    printf("\nOperations :\n\n");

    for (size_t i = 0; i < n; i++)
    {
        int oper = rand() % 30;
        int pos = rand() % n;
        int val = rand() % 100;

        if (oper >= 10)
        {
            if (size == 0) {
                printf("> Insert %d as first", val);
                if(ld_insert_first(liste, sizeof(int), &val) != NULL)  {
                    size++;
                    printf("\t\t\t(Available space : %ld bytes.)\n", ld_total_free_memory(liste));
                } else
                {
                    printf("\t\t\t(Failed...)\n");
                    addMemory(liste);
                }
            }
            else
            {
                int r = rand() % 2;
                if (r == 0)
                {
                    printf("> Insert %d before n°%d", val, pos % size);
                    if (ld_insert_before(liste, ld_get_at(liste, pos % size), sizeof(int), &val) != NULL) {
                        size++;
                        printf("\t\t\t(Available space : %ld bytes.)\n", ld_total_free_memory(liste));
                    } else
                    {
                        printf("\t\t\t(Failed...)\n");
                        addMemory(liste);
                    }
                } else
                {
                    printf("> Insert %d after n°%d", val, pos % size);
                    if (ld_insert_after(liste, ld_get_at(liste, pos % size), sizeof(int), &val) != NULL) {
                        size++;
                        printf("\t\t\t(Available space : %ld bytes.)\n", ld_total_free_memory(liste));
                    } else
                    {
                        printf("\t\t\t(Failed...)\n");
                        addMemory(liste);
                    }
                }
            }
        }
        else if (oper == 0)
        {
            printf("> Compactify memory");
            printf("\t\t\t(Number of nb_blocs : %d ", ld_number_of_free_blocs(liste));
            ld_compactify(liste);
            printf("then %d).\n",ld_number_of_free_blocs(liste));
        }
        else
        {
            int z;
            if (size == 0) z = pos;
            else z = pos % size;
            node* n = ld_get_at(liste, z);
            printf("> Delete node n°%d", z);
            if (ld_delete_node(liste, n) != NULL) {
                size--;
                printf("\t\t\t(Available space : %ld bytes.)\n", ld_total_free_memory(liste));
            } else
            {
                printf("\t\t\t(Failed...)\n");
            }
        }
    }
    printListeState(liste);
    printf("\n");
    print_list_int(liste, 0);
    ld_destroy(liste);
}

static void printUsageAndQuit()
{
    printf("Usage: ./projet2019 --randomTests [number of operations to run] [size in byte to allocate]\n");
    printf("or     ./projet2019\n\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    /* Don't touch this part */
    if (argc != 4 && argc != 1)
    {
        printUsageAndQuit();
    }
    if (argc == 4)
    {
        if (strcmp(argv[1], "--randomTests") != 0)
            printUsageAndQuit();
        int op = atoi(argv[2]);
        size_t spc = atoi(argv[3]);
        if (op <= 0 || spc < 0)
        {
            printf("\nParameters must be integers greater than 0...\n");
            printUsageAndQuit();
        }
        randomTests(op, spc);
    }
    
    /* Write your own tests here */
	

}
