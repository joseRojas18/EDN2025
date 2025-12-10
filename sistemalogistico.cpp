#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;


struct Pedido {
    char destino[30];
    int cantidad;
    Pedido *siguiente;
};


struct Nodo {
    int fecha;
    int stock;
    char nombre_producto[30];

    Pedido *cabeza_pedidos;

    Nodo *izq;
    Nodo *der;
    int altura;
};


int altura(Nodo *n) {
    if (n == NULL) return 0;
    return n->altura;
}

int maximo(int a, int b) {
    return (a > b) ? a : b;
}

int obtenerBalance(Nodo *n) {
    if (n == NULL) return 0;
    return altura(n->izq) - altura(n->der);
}


Nodo* rotacionDerecha(Nodo *y) {
    Nodo *x = y->izq;
    Nodo *T2 = x->der;

    x->der = y;
    y->izq = T2;

    y->altura = maximo(altura(y->izq), altura(y->der)) + 1;
    x->altura = maximo(altura(x->izq), altura(x->der)) + 1;

    return x;
}

Nodo* rotacionIzquierda(Nodo *x) {
    Nodo *y = x->der;
    Nodo *T2 = y->izq;

    y->izq = x;
    x->der = T2;

    x->altura = maximo(altura(x->izq), altura(x->der)) + 1;
    y->altura = maximo(altura(y->izq), altura(y->der)) + 1;

    return y;
}


Nodo* crearNodo(int fecha, int stock, char nombre[]) {
    Nodo *n = (Nodo*)malloc(sizeof(Nodo));
    n->fecha = fecha;
    n->stock = stock;
    strcpy(n->nombre_producto, nombre);
    n->cabeza_pedidos = NULL;
    n->izq = n->der = NULL;
    n->altura = 1;
    return n;
}


Nodo* insertarAVL(Nodo *nodo, int fecha, int stock, char nombre[]) {

    if (nodo == NULL)
        return crearNodo(fecha, stock, nombre);

    if (fecha < nodo->fecha)
        nodo->izq = insertarAVL(nodo->izq, fecha, stock, nombre);
    else if (fecha > nodo->fecha)
        nodo->der = insertarAVL(nodo->der, fecha, stock, nombre);
    else {
        printf("\nla fecha ya existe. no se puede registrar.\n");
        return nodo;
    }

    nodo->altura = 1 + maximo(altura(nodo->izq), altura(nodo->der));
    int balance = obtenerBalance(nodo);

    if (balance > 1 && fecha < nodo->izq->fecha)
        return rotacionDerecha(nodo);

    if (balance < -1 && fecha > nodo->der->fecha)
        return rotacionIzquierda(nodo);

    if (balance > 1 && fecha > nodo->izq->fecha) {
        nodo->izq = rotacionIzquierda(nodo->izq);
        return rotacionDerecha(nodo);
    }

    if (balance < -1 && fecha < nodo->der->fecha) {
        nodo->der = rotacionDerecha(nodo->der);
        return rotacionIzquierda(nodo);
    }

    return nodo;
}


Nodo* buscarMinimoConStock(Nodo *root) {
    if (root == NULL) return NULL;

    Nodo *izq = buscarMinimoConStock(root->izq);
    if (izq != NULL) return izq;

    if (root->stock > 0) return root;

    return buscarMinimoConStock(root->der);
}


Nodo* buscarNodo(Nodo *root, int fecha) {
    if (root == NULL) return NULL;
    if (fecha == root->fecha) return root;
    else if (fecha < root->fecha) return buscarNodo(root->izq, fecha);
    else return buscarNodo(root->der, fecha);
}


void encolar(Pedido **cabeza, char destino[], int cantidad) {
    Pedido *nuevo = (Pedido*)malloc(sizeof(Pedido));
    strcpy(nuevo->destino, destino);
    nuevo->cantidad = cantidad;
    nuevo->siguiente = NULL;

    if (*cabeza == NULL)
        *cabeza = nuevo;
    else {
        Pedido *temp = *cabeza;
        while (temp->siguiente != NULL)
            temp = temp->siguiente;
        temp->siguiente = nuevo;
    }
}


void cancelarPedido(Pedido **cabeza, char destino[]) {
    Pedido *temp = *cabeza;
    Pedido *ant = NULL;

    while (temp != NULL) {
        if (strcmp(temp->destino, destino) == 0) {

            if (ant == NULL)
                *cabeza = temp->siguiente;
            else
                ant->siguiente = temp->siguiente;

            printf("\npedido cancelado: %s\n", destino);
            free(temp);
            return;
        }
        ant = temp;
        temp = temp->siguiente;
    }

    printf("\nno se encontró el pedido.\n");
}


Nodo* buscarMinimo(Nodo *n) {
    while (n->izq != NULL)
        n = n->izq;
    return n;
}

Nodo* eliminarAVL(Nodo *root, int fecha) {

    if (root == NULL) return root;

    if (fecha < root->fecha)
        root->izq = eliminarAVL(root->izq, fecha);
    else if (fecha > root->fecha)
        root->der = eliminarAVL(root->der, fecha);
    else {

        if (root->izq == NULL || root->der == NULL) {
            Nodo *temp = root->izq ? root->izq : root->der;

            if (temp == NULL) {
                temp = root;
                root = NULL;
            }
            free(temp);

        } else {
            Nodo *temp = buscarMinimo(root->der);

            root->fecha = temp->fecha;
            root->stock = temp->stock;
            strcpy(root->nombre_producto, temp->nombre_producto);

            root->der = eliminarAVL(root->der, temp->fecha);
        }
    }

    if (root == NULL) return root;

    root->altura = 1 + maximo(altura(root->izq), altura(root->der));
    int balance = obtenerBalance(root);

    if (balance > 1 && obtenerBalance(root->izq) >= 0)
        return rotacionDerecha(root);

    if (balance > 1 && obtenerBalance(root->izq) < 0) {
        root->izq = rotacionIzquierda(root->izq);
        return rotacionDerecha(root);
    }

    if (balance < -1 && obtenerBalance(root->der) <= 0)
        return rotacionIzquierda(root);

    if (balance < -1 && obtenerBalance(root->der) > 0) {
        root->der = rotacionDerecha(root->der);
        return rotacionIzquierda(root);
    }

    return root;
}


void reporteCompleto(Nodo *root, int espacio = 0) {
    if (root != NULL) {
        reporteCompleto(root->izq, espacio + 8);

        for (int i = 0; i < espacio; i++)
            printf(" ");
        printf("%d (%s) [Stock: %d]\n", root->fecha, root->nombre_producto, root->stock);

        Pedido *p = root->cabeza_pedidos;
        while (p != NULL) {
            for (int i = 0; i < espacio + 4; i++)
                printf(" ");
            printf("-> destino: %s | cantidad: %d\n", p->destino, p->cantidad);
            p = p->siguiente;
        }

        reporteCompleto(root->der, espacio + 8);
    }
}


int main() {

    Nodo *arbol = NULL;
    int opcion;

    while (true) {

        printf("\n==============================================");
        printf("\n   SISTEMA LOGISTICO - PUERTO BUENAVENTURA");
        printf("\n==============================================");
        printf("\n1. recepcion de mercancia");
        printf("\n2. registrar pedido de despacho");
        printf("\n3. cancelaciones");
        printf("\n4. reporte completo");
        printf("\n5. salir");
        printf("\nseleccione opcion: ");
        scanf("%d", &opcion);

       
        if (opcion == 1) {
            int fecha, cantidad;
            char nombre[30];

            printf("\nFecha AAAAMMDD: ");
            scanf("%d", &fecha);
            printf("Nombre producto: ");
            scanf("%s", nombre);
            printf("Cantidad: ");
            scanf("%d", &cantidad);

            arbol = insertarAVL(arbol, fecha, cantidad, nombre);
        }

       
        else if (opcion == 2) {

            Nodo *lote = buscarMinimoConStock(arbol);

            if (lote == NULL) {
                printf("\nno hay productos con stock disponible.\n");
                continue;
            }

            char destino[30];
            int cant;

            printf("\nlote seleccionado: %d (%s) - stock disponible: %d\n", 
                   lote->fecha, lote->nombre_producto, lote->stock);

            printf("ingrese destino: ");
            scanf("%s", destino);
            printf("ingrese cantidad solicitada: ");
            scanf("%d", &cant);

            if (cant > lote->stock) {
                printf("\nno hay suficiente stock en este lote.\n");
            } else {
                encolar(&lote->cabeza_pedidos, destino, cant);
                lote->stock -= cant;
                printf("\npedido registrado correctamente.\n");
            }
        }

     
        else if (opcion == 3) {

            int tipo;
            printf("\n1. Baja de producto completo");
            printf("\n2. Cancelar pedido");
            printf("\nseleccione: ");
            scanf("%d", &tipo);

            if (tipo == 1) {
                int fecha;
                printf("\nfecha del lote a eliminar: ");
                scanf("%d", &fecha);
                arbol = eliminarAVL(arbol, fecha);
                printf("\nlote eliminado.\n");
            } else if (tipo == 2) {
                int fecha;
                char destino[30];

                printf("\nlecha del lote: ");
                scanf("%d", &fecha);

                Nodo *temp = buscarNodo(arbol, fecha);

                if (temp == NULL) {
                    printf("\nlote no encontrado.\n");
                } else {
                    printf("destino del pedido a cancelar: ");
                    scanf("%s", destino);
                    cancelarPedido(&temp->cabeza_pedidos, destino);
                }
            }
        }

       
        else if (opcion == 4) {
            printf("\n--- REPORTE COMPLETO DE INVENTARIO Y PEDIDOS ---\n");
            reporteCompleto(arbol);
        }

        else if (opcion == 5) {
            break;
        }
    }

    return 0;
}
