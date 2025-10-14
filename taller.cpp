#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOMBRE 50

#ifndef STRUCTS
#define STRUCTS


typedef struct pasajero {
    char genero[20];
    char apellido[MAX_NOMBRE];
    struct pasajero *siguiente;
} Pasajero;

#endif

Pasajero *inicio = NULL; 
Pasajero *abordados = NULL; 
Pasajero *noAbordados = NULL; 

int capacidad = 0;
int totalVendidos = 0;
int abordajeIniciado = 0;


Pasajero *crearPasajero(char genero[], char apellido[]) {
    Pasajero *nuevo = (Pasajero *)malloc(sizeof(Pasajero));
    if (nuevo == NULL) {
        printf("Error: no se pudo asignar memoria.\n");
        exit(1);
    }
    strcpy(nuevo->genero, genero);
    strcpy(nuevo->apellido, apellido);
    nuevo->siguiente = NULL;
    return nuevo;
}


void agregarPasajero(Pasajero **lista, Pasajero *nuevo) {
    if (*lista == NULL) {
        *lista = nuevo;
    } else {
        Pasajero *temp = *lista;
        while (temp->siguiente != NULL) {
            temp = temp->siguiente;
        }
        temp->siguiente = nuevo;
    }
}


void mostrarLista(Pasajero *lista) {
    if (lista == NULL) {
        printf("No hay pasajeros en esta lista.\n");
        return;
    }
    Pasajero *temp = lista;
    int cont = 1;
    while (temp != NULL) {
        printf("%d. %s - %s\n", cont, temp->apellido, temp->genero);
        temp = temp->siguiente;
        cont++;
    }
}


int contarLista(Pasajero *lista) {
    int c = 0;
    Pasajero *temp = lista;
    while (temp != NULL) {
        c++;
        temp = temp->siguiente;
    }
    return c;
}


void establecerCapacidad() {
    if (capacidad != 0) {
        printf("La capacidad ya fue establecida.\n");
        return;
    }
    printf("Ingrese la capacidad maxima del avion: ");
    scanf("%d", &capacidad);
    if (capacidad <= 0) {
        printf("Capacidad invalida. Se debe ingresar un numero mayor que 0.\n");
        capacidad = 0;
        return;
    }
    printf("Capacidad establecida en %d pasajeros.\n", capacidad);
}


void venderTiquete() {
    if (capacidad == 0) {
        printf("Primero debe establecer la capacidad del avion.\n");
        return;
    }
    if (abordajeIniciado) {
        printf("Ya inicio el abordaje, no se pueden vender mas tiquetes.\n");
        return;
    }

    
    int limite = capacidad + (capacidad + 9) / 10;
    if (totalVendidos >= limite) {
        printf("Se alcanzo el limite de sobreventa (%d pasajeros).\n", limite);
        return;
    }

    char genero[20], apellido[MAX_NOMBRE];
    printf("Ingrese genero (Femenino / Masculino / No Binario): ");
    scanf("%19s", genero);
    printf("Ingrese primer apellido: ");
    scanf("%49s", apellido);

    Pasajero *nuevo = crearPasajero(genero, apellido);
    agregarPasajero(&inicio, nuevo);
    totalVendidos++;
    printf("Tiquete vendido con exito.\n");
}


void iniciarAbordaje() {
    if (abordajeIniciado) {
        printf("El abordaje ya se inicio.\n");
        return;
    }
    if (inicio == NULL) {
        printf("No hay pasajeros para abordar.\n");
        return;
    }

    abordajeIniciado = 1;
    int contador = 0;
    Pasajero *temp = inicio;

    while (temp != NULL) {
        Pasajero *nuevo = crearPasajero(temp->genero, temp->apellido);
        if (contador < capacidad) {
            agregarPasajero(&abordados, nuevo);
        } else {
            agregarPasajero(&noAbordados, nuevo);
        }
        temp = temp->siguiente;
        contador++;
    }

    int boarded = contarLista(abordados);
    int noAb = (totalVendidos > boarded) ? (totalVendidos - boarded) : 0;
    int subieron = (totalVendidos < capacidad) ? totalVendidos : boarded;
    printf("El abordaje ha iniciado. %d pasajeros subieron, %d quedaron fuera.\n",
           subieron, noAb);
}


void verNoAbordados() {
    if (!abordajeIniciado) {
        printf("\nAun no se ha iniciado el abordaje, no hay lista disponible.\n");
        return;
    }

    
    int boarded = contarLista(abordados);

    
    if (totalVendidos >= capacidad) {
        int quedaronFuera = totalVendidos - boarded; 
        
        if (boarded == 1)
            printf("\n1 pasajero abordo, %d quedaron por fuera.\n", quedaronFuera);
        else
            printf("\n%d pasajeros abordaron, %d quedaron por fuera.\n", boarded, quedaronFuera);

        
        if (noAbordados != NULL) {
            printf("\n--- LISTA: Pasajeros que no abordaron ---\n");
            mostrarLista(noAbordados);
        } else {
            if (quedaronFuera == 0) {
                printf("No hay pasajeros que no abordaron.\n");
            } else {
                
                printf("\n--- LISTA: Pasajeros que no abordaron (generada) ---\n");
                Pasajero *temp = inicio;
                int idx = 0;
                int cont = 1;
                while (temp != NULL) {
                    if (idx >= capacidad) {
                        printf("%d. %s - %s\n", cont, temp->apellido, temp->genero);
                        cont++;
                    }
                    temp = temp->siguiente;
                    idx++;
                }
            }
        }
        return;
    }

    
    int asientosVacios = capacidad - boarded;
    if (boarded == 1)
        printf("\n1 pasajero abordo, %d quedaron por fuera.\n", asientosVacios);
    else
        printf("\n%d pasajeros abordaron, %d quedaron por fuera.\n", boarded, asientosVacios);

    
    if (asientosVacios > 0) {
        printf("Nota: %d asientos quedan vacios.\n", asientosVacios);
    }
}


int main() {
    int opcion = 0;

    do {
        printf("\n==== MENU PRINCIPAL ====\n");
        printf("1. Establecer Capacidad\n");
        printf("2. Vender Tiquete\n");
        printf("3. Iniciar Abordaje\n");
        printf("4. Ver Abordados\n");
        printf("5. Ver No Abordados\n");
        printf("6. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                establecerCapacidad();
                break;
            case 2:
                venderTiquete();
                break;
            case 3:
                iniciarAbordaje();
                break;
            case 4:
                printf("\n--- PASAJEROS ABORDADOS ---\n");
                mostrarLista(abordados);
                break;
            case 5:
                verNoAbordados();
                break;
            case 6:
                printf("Saliendo del programa...\n");
                break;
            default:
                printf("Opcion no valida.\n");
        }
    } while (opcion != 6);

    
    Pasajero *temp;
    while (inicio != NULL) {
        temp = inicio;
        inicio = inicio->siguiente;
        free(temp);
    }
    while (abordados != NULL) {
        temp = abordados;
        abordados = abordados->siguiente;
        free(temp);
    }
    while (noAbordados != NULL) {
        temp = noAbordados;
        noAbordados = noAbordados->siguiente;
        free(temp);
    }

    return 0;
}
