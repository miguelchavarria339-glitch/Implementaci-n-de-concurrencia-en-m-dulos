#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

using namespace std;

// Almacén compartido
queue<int> almacen;

// Capacidad máxima del almacén
const int TAMANO = 5;

// Mutex para proteger el almacén
mutex mtx;

// Variables para controlar cuándo producir y consumir
condition_variable puede_producir;
condition_variable puede_consumir;


// =========================
// PRODUCTOR
// =========================
void productor(int id, int inicio) {

    for (int i = 0; i < 5; i++) {

        // Bloqueamos el almacén
        unique_lock<mutex> lock(mtx);

        // Esperar si el almacén está lleno
        puede_producir.wait(lock, [] {
            return almacen.size() < TAMANO;
        });

        // Crear un producto
        int producto = inicio + i;

        // Guardar el producto
        almacen.push(producto);

        cout << "Productor " << id
             << ": produjo producto "
             << producto << endl;

        // Liberar el almacén
        lock.unlock();

        // Avisar que hay un producto disponible
        puede_consumir.notify_one();

        // Pequeña pausa
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}


// =========================
// CONSUMIDOR
// =========================
void consumidor() {

    // Consumirá 10 productos
    for (int i = 0; i < 10; i++) {

        // Bloqueamos el almacén
        unique_lock<mutex> lock(mtx);

        // Esperar si el almacén está vacío
        puede_consumir.wait(lock, [] {
            return !almacen.empty();
        });

        // Obtener el primer producto
        int producto = almacen.front();

        // Eliminarlo del almacén
        almacen.pop();

        cout << "Consumidor: consumio producto "
             << producto << endl;

        // Liberar el almacén
        lock.unlock();

        // Avisar que hay espacio disponible
        puede_producir.notify_one();

        // Pequeña pausa
        this_thread::sleep_for(chrono::milliseconds(800));
    }
}


// =========================
// PROGRAMA PRINCIPAL
// =========================
int main() {

    // Crear 3 hilos concurrentes

    thread productor1(productor, 1, 1);

    thread productor2(productor, 2, 101);

    thread consumidor1(consumidor);


    // Esperar a que terminen los hilos

    productor1.join();

    productor2.join();

    consumidor1.join();


    cout << "\nTodos los procesos terminaron." << endl;

    return 0;
}