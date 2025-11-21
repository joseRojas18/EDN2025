#include <iostream>
#include <string.h>
#include <stdlib.h>

using namespace std;

struct Nodo {
    int doc;
    char destino[20];
    int tipo; 
    Nodo *izq, *der;
};


Nodo* crear(int d, const char* dest, int t) {
    Nodo* n = new Nodo;
    n->doc = d;
    strncpy(n->destino, dest, sizeof(n->destino)-1); n->destino[sizeof(n->destino)-1] = '\0';
    n->tipo = t;
    n->izq = n->der = nullptr;
    return n;
}

Nodo* insertar(Nodo* r, int d, const char* dest, int t) {
    if(!r) return crear(d, dest, t);
    if(d < r->doc) r->izq = insertar(r->izq, d, dest, t);
    else if(d > r->doc) r->der = insertar(r->der, d, dest, t);
    return r;
}

void imprimirPasajero(Nodo* r) {
    if(r) {
        cout << r->doc << " | " << r->destino << " | " << ((r->tipo==0) ? "Ida" : "Ida y Regreso") << "\n";
    }
}

void inorden(Nodo* r){ if(r){ inorden(r->izq); imprimirPasajero(r); inorden(r->der);} }
void preorden(Nodo* r){ if(r){ imprimirPasajero(r); preorden(r->izq); preorden(r->der);} }
void postorden(Nodo* r){ if(r){ postorden(r->izq); postorden(r->der); imprimirPasajero(r);} }

int contar(Nodo* r){ return r ? 1 + contar(r->izq) + contar(r->der) : 0; }
Nodo* minimo(Nodo* r){ while(r->izq) r=r->izq; return r; }

Nodo* eliminar(Nodo* r, int d) {
    if(!r) return nullptr;
    if(d < r->doc) r->izq = eliminar(r->izq, d);
    else if(d > r->doc) r->der = eliminar(r->der, d);
    else {
        if(!r->izq){ Nodo* tmp=r->der; delete r; return tmp; }
        if(!r->der){ Nodo* tmp=r->izq; delete r; return tmp; }
        Nodo* tmp = minimo(r->der);
        r->doc = tmp->doc;
        strncpy(r->destino, tmp->destino, sizeof(r->destino)-1); r->destino[sizeof(r->destino)-1]='\0';
        r->tipo = tmp->tipo;
        r->der = eliminar(r->der, tmp->doc);
    }
    return r;
}


void titulo() {
    system("cls"); 
    cout << "===========================================\n";
    cout << " SISTEMA DE TIQUETES - MUELLE BUENAVENTURA \n";
    cout << "===========================================\n\n";
}

void menu() {
    cout << "MENU PRINCIPAL\n";
    cout << "1. Registrar pasajero\n";
    cout << "2. Mostrar recorrido INORDEN\n";
    cout << "3. Mostrar recorrido PREORDEN\n";
    cout << "4. Mostrar recorrido POSTORDEN\n";
    cout << "5. Contar pasajeros\n";
    cout << "6. Eliminar pasajero\n";
    cout << "7. Salir\n";
    cout << "Opcion: ";
}


int main() {
    Nodo* raiz = nullptr;
    int op;
    const char* dnames[] = {"", "Timbiqui", "Juanchaco", "Tumaco", "Guapi"};

    do {
        titulo();
        menu();
        cin >> op;

        if(op == 1) {
            int doc, tipoOp, destOp;
            char destino[20];

            titulo();
            cout << "Registro de Pasajero\n";
            cout << "Documento: "; cin >> doc;

            cout << "\nDestino:\n 1) Timbiqui\n 2) Juanchaco\n 3) Tumaco\n 4) Guapi\nOpción: ";
            cin >> destOp;
            if(destOp < 1 || destOp > 4) destOp = 1;
            strncpy(destino, dnames[destOp], sizeof(destino)-1); destino[sizeof(destino)-1]='\0';

            cout << "\nTipo de pasaje:\n 0) Ida\n 1) Ida y Regreso\nOpción: ";
            cin >> tipoOp;
            if(tipoOp != 0 && tipoOp != 1) tipoOp = 0;

            raiz = insertar(raiz, doc, destino, tipoOp);

            cout << "\nPasajero registrado correctamente.\n";
            system("pause");
        }

        else if(op >=2 && op <=4) {
            titulo();
            if(op==2){ cout << "RECORRIDO INORDEN:\n"; inorden(raiz); }
            if(op==3){ cout << "RECORRIDO PREORDEN:\n"; preorden(raiz); }
            if(op==4){ cout << "RECORRIDO POSTORDEN:\n"; postorden(raiz); }
            cout << "\n";
            system("pause");
        }

        else if(op == 5) {
            titulo();
            cout << "Total pasajeros: " << contar(raiz) << "\n\n";
            system("pause");
        }

        else if(op == 6) {
            int d;
            titulo();
            cout << "Documento a eliminar: "; cin >> d;
            raiz = eliminar(raiz, d);
            cout << "\nPasajero eliminado si existía.\n";
            system("pause");
        }

    } while(op != 7);

    titulo();
    cout << "Gracias por usar el sistema.\n";
    return 0;
}
