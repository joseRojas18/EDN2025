#include <iostream>
using namespace std;

struct Estudiante {
    int codigo;
    float nota;
};

Estudiante lista[] = {
    {2024101, 4.5},
    {2024102, 3.2},
    {2024103, 5.0},
    {2024104, 2.8},
    {2024105, 4.0}
};

int n = 5;

// Nota máxima recursiva
float maxNota(int i) {
    if (i == n-1) return lista[i].nota;
    float maxResto = maxNota(i+1);
    return (lista[i].nota > maxResto) ? lista[i].nota : maxResto;
}

// Promedio recursivo
float promedio(int i) {
    if (i == n-1) return lista[i].nota / n;
    return lista[i].nota / n + promedio(i+1);
}

// Intercambiar usando punteros
void swap(Estudiante* a, Estudiante* b) {
    Estudiante temp = *a;
    *a = *b;
    *b = temp;
}

// Selection sort recursivo
void selectionSort(int start) {
    if (start >= n-1) return;
    int min = start;
    for (int i = start+1; i<n; i++)
        if (lista[i].codigo < lista[min].codigo) min = i;
    if (min != start) swap(&lista[start], &lista[min]);
    selectionSort(start+1);
}

// Mostrar lista
void mostrar() {
    for (int i=0; i<n; i++)
        cout << "Codigo: " << lista[i].codigo << " Nota: " << lista[i].nota << endl;
}

int main() {
    int op;
    do {
        cout << "\nMenu:\n1. Nota maxima\n2. Promedio\n3. Ordenar codigos\n4. Salir\nElige: ";
        cin >> op;
        switch(op) {
            case 1: cout << "Nota maxima: " << maxNota(0) << endl; break;
            case 2: cout << "Promedio: " << promedio(0) << endl; break;
            case 3: selectionSort(0); mostrar(); break;
            case 4: cout << "Adios!\n"; break;
            default: cout << "Opcion invalida\n";
        }
    } while(op != 4);
    return 0;
}
