/* Sistema de Gestión de Tiquetes - Muelle Turístico de Buenaventura
   Implementación de ABB (Árbol Binario de Búsqueda) en C
   Librerías usadas: stdio.h, stdlib.h, string.h
   Autor: (taller)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------
   Definiciones y estructuras
   ------------------------- */

/* Estructura Pasajero (nodo del ABB) */
typedef struct Pasajero {
    int documento;              /* clave de búsqueda */
    char destino[32];           /* destino */
    int tipo_pasaje;            /* 0 = Ida, 1 = Ida y Regreso */
    struct Pasajero *izq;
    struct Pasajero *der;
} Pasajero;

/* Pila para recorridos iterativos (almacena punteros a nodos) */
typedef struct {
    Pasajero **items;
    int top;
    int capacity;
} Pila;

/* Cola para recorrido por niveles (BFS) */
typedef struct NodoCola {
    Pasajero *pas;
    struct NodoCola *siguiente;
} NodoCola;

typedef struct {
    NodoCola *frente;
    NodoCola *fin;
} Cola;

/* -------------------------
   Prototipos
   ------------------------- */

/* Operaciones ABB */
Pasajero* crearPasajero(int documento, const char *destino, int tipo_pasaje);
Pasajero* insertarAbb(Pasajero *raiz, Pasajero *nuevo);
Pasajero* eliminarAbb(Pasajero *raiz, int documento, int *encontrado);
Pasajero* buscarMin(Pasajero *raiz);

/* Recorridos (recursivos) */
void inorden(Pasajero *raiz);
void preorden(Pasajero *raiz);
void postorden(Pasajero *raiz);

/* Recorridos (iterativos usando pila/cola) */
void preordenIterativo(Pasajero *raiz);
void inordenIterativo(Pasajero *raiz);
void recorridoPorNiveles(Pasajero *raiz);

/* Conteo */
int contarNodos(Pasajero *raiz);

/* Pila (para iterativos) */
Pila* crearPila(int capacity);
void push(Pila *p, Pasajero *valor);
Pasajero* pop(Pila *p);
int pilaVacia(Pila *p);
void liberarPila(Pila *p);

/* Cola */
Cola* crearCola();
void encolar(Cola *q, Pasajero *p);
Pasajero* desencolar(Cola *q);
int colaVacia(Cola *q);
void liberarCola(Cola *q);

/* Utilidades y menú */
void mostrarPasajero(Pasajero *p);
int elegirDestino(char *destino_out);
void limpiarBuffer();
void liberarArbol(Pasajero *raiz);
void menu();

/* -------------------------
   Implementación
   ------------------------- */

/* Crear nodo pasajero */
Pasajero* crearPasajero(int documento, const char *destino, int tipo_pasaje) {
    Pasajero *p = (Pasajero*) malloc(sizeof(Pasajero));
    if (!p) {
        printf("Error: memoria insuficiente.\n");
        exit(EXIT_FAILURE);
    }
    p->documento = documento;
    strncpy(p->destino, destino, sizeof(p->destino)-1);
    p->destino[sizeof(p->destino)-1] = '\0';
    p->tipo_pasaje = tipo_pasaje;
    p->izq = p->der = NULL;
    return p;
}

/* Insertar en ABB: si documento ya existe, NO inserta */
Pasajero* insertarAbb(Pasajero *raiz, Pasajero *nuevo) {
    if (raiz == NULL) {
        return nuevo;
    }
    if (nuevo->documento < raiz->documento) {
        raiz->izq = insertarAbb(raiz->izq, nuevo);
    } else if (nuevo->documento > raiz->documento) {
        raiz->der = insertarAbb(raiz->der, nuevo);
    } else {
        /* documento repetido: ignorar */
        printf("Registro ignorado: el documento %d ya existe.\n", nuevo->documento);
        free(nuevo);
    }
    return raiz;
}

/* Buscar mínimo en subárbol (usado en eliminación) */
Pasajero* buscarMin(Pasajero *raiz) {
    Pasajero *actual = raiz;
    while (actual && actual->izq != NULL)
        actual = actual->izq;
    return actual;
}

/* Eliminar nodo por documento */
Pasajero* eliminarAbb(Pasajero *raiz, int documento, int *encontrado) {
    if (raiz == NULL) {
        *encontrado = 0;
        return NULL;
    }
    if (documento < raiz->documento) {
        raiz->izq = eliminarAbb(raiz->izq, documento, encontrado);
    } else if (documento > raiz->documento) {
        raiz->der = eliminarAbb(raiz->der, documento, encontrado);
    } else {
        /* Encontrado */
        *encontrado = 1;
        /* Caso 1: sin hijos o un hijo */
        if (raiz->izq == NULL) {
            Pasajero *temp = raiz->der;
            free(raiz);
            return temp;
        } else if (raiz->der == NULL) {
            Pasajero *temp = raiz->izq;
            free(raiz);
            return temp;
        }
        /* Caso 2: dos hijos -> reemplazar por sucesor (mínimo del subárbol derecho) */
        Pasajero *sucesor = buscarMin(raiz->der);
        raiz->documento = sucesor->documento;
        strncpy(raiz->destino, sucesor->destino, sizeof(raiz->destino)-1);
        raiz->tipo_pasaje = sucesor->tipo_pasaje;
        raiz->der = eliminarAbb(raiz->der, sucesor->documento, encontrado);
    }
    return raiz;
}

/* Recorridos recursivos: fáciles de entender para principiantes */
void inorden(Pasajero *raiz) {
    if (raiz == NULL) return;
    inorden(raiz->izq);
    mostrarPasajero(raiz);
    inorden(raiz->der);
}

void preorden(Pasajero *raiz) {
    if (raiz == NULL) return;
    mostrarPasajero(raiz);
    preorden(raiz->izq);
    preorden(raiz->der);
}

void postorden(Pasajero *raiz) {
    if (raiz == NULL) return;
    postorden(raiz->izq);
    postorden(raiz->der);
    mostrarPasajero(raiz);
}

/* -------------------------
   PILA: implementación simple con arreglo dinámico
   ------------------------- */

Pila* crearPila(int capacity) {
    Pila *p = (Pila*) malloc(sizeof(Pila));
    if (!p) { printf("Error memoria pila\n"); exit(EXIT_FAILURE); }
    p->items = (Pasajero**) malloc(sizeof(Pasajero*) * capacity);
    if (!p->items) { printf("Error memoria pila items\n"); exit(EXIT_FAILURE); }
    p->top = -1;
    p->capacity = capacity;
    return p;
}

void push(Pila *p, Pasajero *valor) {
    if (p->top + 1 >= p->capacity) {
        /* redimensionar */
        int nueva = p->capacity * 2;
        Pasajero **tmp = (Pasajero**) realloc(p->items, sizeof(Pasajero*) * nueva);
        if (!tmp) { printf("Error: realloc pila\n"); exit(EXIT_FAILURE); }
        p->items = tmp;
        p->capacity = nueva;
    }
    p->items[++(p->top)] = valor;
}

Pasajero* pop(Pila *p) {
    if (p->top == -1) return NULL;
    return p->items[(p->top)--];
}

int pilaVacia(Pila *p) {
    return p->top == -1;
}

void liberarPila(Pila *p) {
    if (p) {
        free(p->items);
        free(p);
    }
}

/* -------------------------
   COLA: implementación con lista enlazada
   ------------------------- */

Cola* crearCola() {
    Cola *q = (Cola*) malloc(sizeof(Cola));
    if (!q) { printf("Error memoria cola\n"); exit(EXIT_FAILURE); }
    q->frente = q->fin = NULL;
    return q;
}

void encolar(Cola *q, Pasajero *p) {
    NodoCola *n = (NodoCola*) malloc(sizeof(NodoCola));
    if (!n) { printf("Error memoria nodo cola\n"); exit(EXIT_FAILURE); }
    n->pas = p;
    n->siguiente = NULL;
    if (q->fin == NULL) {
        q->frente = q->fin = n;
    } else {
        q->fin->siguiente = n;
        q->fin = n;
    }
}

Pasajero* desencolar(Cola *q) {
    if (q->frente == NULL) return NULL;
    NodoCola *temp = q->frente;
    Pasajero *p = temp->pas;
    q->frente = q->frente->siguiente;
    if (q->frente == NULL) q->fin = NULL;
    free(temp);
    return p;
}

int colaVacia(Cola *q) {
    return q->frente == NULL;
}

void liberarCola(Cola *q) {
    while (!colaVacia(q)) {
        desencolar(q);
    }
    free(q);
}

/* -------------------------
   Recorridos iterativos usando pilas y colas
   ------------------------- */

/* Preorden iterativo (usa pila) */
void preordenIterativo(Pasajero *raiz) {
    if (!raiz) {
        printf("[Arbol vacío]\n");
        return;
    }
    Pila *p = crearPila(32);
    push(p, raiz);
    while (!pilaVacia(p)) {
        Pasajero *n = pop(p);
        mostrarPasajero(n);
        /* push derecha primero para procesar izquierda antes */
        if (n->der) push(p, n->der);
        if (n->izq) push(p, n->izq);
    }
    liberarPila(p);
}

/* Inorden iterativo (usa pila) */
void inordenIterativo(Pasajero *raiz) {
    if (!raiz) {
        printf("[Arbol vacío]\n");
        return;
    }
    Pila *p = crearPila(32);
    Pasajero *actual = raiz;
    while (actual != NULL || !pilaVacia(p)) {
        while (actual != NULL) {
            push(p, actual);
            actual = actual->izq;
        }
        actual = pop(p);
        mostrarPasajero(actual);
        actual = actual->der;
    }
    liberarPila(p);
}

/* Recorrido por niveles (BFS) usando cola) */
void recorridoPorNiveles(Pasajero *raiz) {
    if (!raiz) {
        printf("[Arbol vacío]\n");
        return;
    }
    Cola *q = crearCola();
    encolar(q, raiz);
    while (!colaVacia(q)) {
        Pasajero *n = desencolar(q);
        mostrarPasajero(n);
        if (n->izq) encolar(q, n->izq);
        if (n->der) encolar(q, n->der);
    }
    liberarCola(q);
}

/* -------------------------
   Conteo de nodos (total de pasajeros)
   ------------------------- */

int contarNodos(Pasajero *raiz) {
    if (raiz == NULL) return 0;
    return 1 + contarNodos(raiz->izq) + contarNodos(raiz->der);
}

/* -------------------------
   Utilidades
   ------------------------- */

void mostrarPasajero(Pasajero *p) {
    if (!p) return;
    printf("Documento: %d | Destino: %s | Tipo: %s\n",
           p->documento,
           p->destino,
           (p->tipo_pasaje == 0) ? "Ida" : "Ida y Regreso");
}

/* Lista predefinida de destinos.
   Devuelve 1 si se seleccionó un destino válido y copia en destino_out,
   0 en caso contrario. */
int elegirDestino(char *destino_out) {
    const char *destinos[] = {"Timbiqui", "Juanchaco", "Tumaco", "Guapi"};
    int n = sizeof(destinos)/sizeof(destinos[0]);
    printf("Destinos disponibles:\n");
    for (int i = 0; i < n; ++i) {
        printf(" %d) %s\n", i+1, destinos[i]);
    }
    printf("Seleccione destino (numero): ");
    int opt;
    if (scanf("%d", &opt) != 1) {
        limpiarBuffer();
        return 0;
    }
    limpiarBuffer();
    if (opt < 1 || opt > n) {
        printf("Opción inválida.\n");
        return 0;
    }
    strncpy(destino_out, destinos[opt-1], 31);
    destino_out[31] = '\0';
    return 1;
}

/* Limpiar buffer de entrada (evita lecturas duplicadas) */
void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* Liberar todo el árbol (al salir) */
void liberarArbol(Pasajero *raiz) {
    if (raiz == NULL) return;
    liberarArbol(raiz->izq);
    liberarArbol(raiz->der);
    free(raiz);
}

/* -------------------------
   Menú interactivo
   ------------------------- */

void menu() {
    Pasajero *raiz = NULL;
    int opcion = 0;
    do {
        printf("\n=== Sistema de Tiquetes - Muelle Turístico de Buenaventura ===\n");
        printf("1) Registrar pasajero (insertar)\n");
        printf("2) Listar pasajeros - Inorden (recursivo)\n");
        printf("3) Listar pasajeros - Preorden (recursivo)\n");
        printf("4) Listar pasajeros - Postorden (recursivo)\n");
        printf("5) Listar pasajeros - Inorden (iterativo, con pila)\n");
        printf("6) Listar pasajeros - Preorden (iterativo, con pila)\n");
        printf("7) Listar pasajeros - Por niveles (cola, BFS)\n");
        printf("8) Contar pasajeros\n");
        printf("9) Eliminar pasajero por documento\n");
        printf("10) Salir\n");
        printf("Seleccione una opción: ");
        if (scanf("%d", &opcion) != 1) {
            printf("Entrada inválida.\n");
            limpiarBuffer();
            continue;
        }
        limpiarBuffer();

        if (opcion == 1) {
            int doc;
            char destino[32];
            int tipo;
            printf("Ingrese número de documento (entero): ");
            if (scanf("%d", &doc) != 1) {
                printf("Documento inválido.\n");
                limpiarBuffer();
                continue;
            }
            limpiarBuffer();
            if (!elegirDestino(destino)) continue;
            printf("Tipo de pasaje: 1) Ida   2) Ida y Regreso\n");
            printf("Seleccione (1 o 2): ");
            if (scanf("%d", &tipo) != 1) {
                printf("Entrada inválida.\n");
                limpiarBuffer();
                continue;
            }
            limpiarBuffer();
            if (tipo != 1 && tipo != 2) {
                printf("Tipo inválido.\n");
                continue;
            }
            Pasajero *nuevo = crearPasajero(doc, destino, (tipo==1)?0:1);
            raiz = insertarAbb(raiz, nuevo);
            printf("Registro intentado para documento %d.\n", doc);
        }
        else if (opcion == 2) {
            printf("\n--- Inorden (recursivo) ---\n");
            if (!raiz) printf("[Arbol vacío]\n");
            else inorden(raiz);
        }
        else if (opcion == 3) {
            printf("\n--- Preorden (recursivo) ---\n");
            if (!raiz) printf("[Arbol vacío]\n");
            else preorden(raiz);
        }
        else if (opcion == 4) {
            printf("\n--- Postorden (recursivo) ---\n");
            if (!raiz) printf("[Arbol vacío]\n");
            else postorden(raiz);
        }
        else if (opcion == 5) {
            printf("\n--- Inorden (iterativo con pila) ---\n");
            inordenIterativo(raiz);
        }
        else if (opcion == 6) {
            printf("\n--- Preorden (iterativo con pila) ---\n");
            preordenIterativo(raiz);
        }
        else if (opcion == 7) {
            printf("\n--- Recorrido por niveles (BFS con cola) ---\n");
            recorridoPorNiveles(raiz);
        }
        else if (opcion == 8) {
            int total = contarNodos(raiz);
            printf("Total de pasajeros registrados: %d\n", total);
        }
        else if (opcion == 9) {
            printf("Ingrese documento a eliminar: ");
            int doc;
            if (scanf("%d", &doc) != 1) {
                printf("Entrada inválida.\n");
                limpiarBuffer();
                continue;
            }
            limpiarBuffer();
            int encontrado = 0;
            raiz = eliminarAbb(raiz, doc, &encontrado);
            if (encontrado) printf("Documento %d eliminado.\n", doc);
            else printf("Documento %d no encontrado.\n", doc);
        }
        else if (opcion == 10) {
            printf("Saliendo... liberando memoria.\n");
            liberarArbol(raiz);
        }
        else {
            printf("Opción no válida. Intente de nuevo.\n");
        }

    } while (opcion != 10);
}

/* -------------------------
   main
   ------------------------- */

int main() {
    /* Mensaje introductorio */
    printf("Bienvenido al Sistema de Gestión de Tiquetes - Muelle Turístico de Buenaventura\n");
    printf("Este programa usa un Árbol Binario de Búsqueda (ABB) indexado por documento.\n");
    printf("Se incluyen pilas y colas para recorridos iterativos y por niveles.\n");

    menu();

    return 0;
}
