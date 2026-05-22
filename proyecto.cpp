#include <iostream>
#include <string>
#include <limits> 
#include <cctype> 
#include <cstdlib> // Para rand()
#include <ctime>   // Para semilla del generador

using namespace std;

// Constantes globales
const int MAX_HP_BASE = 400;
const int MAX_CAPACIDAD_GLOBAL = 60;

// Contadores globales
int contadorNeon = 0;
int contadorOmega = 0;

/*==========================================================
1. ESTRUCTURAS DE EQUIPOS 
==========================================================*/
struct EquipoNeon {
    int Bando = 1;
    string nombre = "Resistencia Neon";
};

struct EquipoOmega {
    int Bando = 2;
    string nombre = "Corporacion OMEGA";
};

/*==========================================================
2. TDA PILADEFENSA (STACK LIFO)
============================================================*/
struct NodoDefensa {
    string tipo_escudo;
    int puntos_defensa;
    NodoDefensa* siguiente = nullptr;
};
typedef NodoDefensa* PilaDefensa; // Puntero al tope de la pila

void apilarEscudo(PilaDefensa &pila, string tipo, int puntos) {
    NodoDefensa* nuevo = new NodoDefensa{tipo, puntos, pila};
    pila = nuevo;
}

void desapilarEscudo(PilaDefensa &pila) {
    if (pila != nullptr) {
        NodoDefensa* aux = pila;
        pila = pila->siguiente;
        delete aux;
    }
}

/*==========================================================
3. TDA COLAARSENAL (QUEUE FIFO) 
============================================================*/
struct NodoMunicion { // Blast Especial
    int id_blast;
    string tipo_blast;
    int dano;
    NodoMunicion* siguiente = nullptr;
};

struct ColaArsenal {
    NodoMunicion* frente = nullptr;
    NodoMunicion* final = nullptr;
};

void encolarBlast(ColaArsenal &cola, int id, string tipo, int dano) {
    NodoMunicion* nuevo = new NodoMunicion{id, tipo, dano, nullptr};
    if (cola.final == nullptr) {
        cola.frente = nuevo;
        cola.final = nuevo;
    } else {
        cola.final->siguiente = nuevo;
        cola.final = nuevo;
    }
}

void desencolarBlast(ColaArsenal &cola) {
    if (cola.frente != nullptr) {
        NodoMunicion* aux = cola.frente;
        cola.frente = cola.frente->siguiente;
        if (cola.frente == nullptr) cola.final = nullptr;
        delete aux;
    }
}

struct NodoItem {
    string nombre_item;
    string tipo_efecto; 
    int valor_efecto;   
    NodoItem* siguiente = nullptr;
};

/*==========================================================
4. TDA UNIDAD (EL OPERATIVO)
============================================================*/
struct Personaje {
    string clase;
    int HP_Base;
    int HP_Base_Max; // Mecanica de curacion
    int dano_base;   // Arma Base de la clase
    PilaDefensa escudos = nullptr;  
    ColaArsenal municiones;         
    string habilidad; 
    NodoItem* inventario = nullptr; 
};

void generarSuministros(ColaArsenal &cola, string clase) {
    int cantidad = rand() % 4 + 1; // 1 a 4 Blasts aleatorios
    for (int i = 0; i < cantidad; i++) {
        int r = rand() % 5 + 1;
        if (r == 1) encolarBlast(cola, 1, "Laser Estandar", 25);
        else if (r == 2) encolarBlast(cola, 2, "Perforante (EMP)", 50);
        else if (r == 3 && clase == "Espectro") encolarBlast(cola, 3, "Troyano", 0); // Solo Espectro
        else if (r == 3 && clase != "Espectro") encolarBlast(cola, 1, "Laser Estandar", 25); // Relleno
        else if (r == 4) encolarBlast(cola, 4, "Disrupcion", 0);
        else if (r == 5) encolarBlast(cola, 5, "Racimo (AoE)", 20);
    }
}

Personaje obtenerPlantillaPersonaje(int tipo) {
    Personaje p;
    p.municiones.frente = nullptr; p.municiones.final = nullptr;

    if (tipo == 1) { // Juggernaut
        p.clase = "Juggernaut"; p.HP_Base = 150; p.HP_Base_Max = 150; p.dano_base = 15; p.habilidad = "Fuerza Bruta";
        apilarEscudo(p.escudos, "Escudo Fisico", 50);
        apilarEscudo(p.escudos, "Escudo Fisico", 50);
        apilarEscudo(p.escudos, "Escudo Fisico", 50); // Nace con 3 escudos
    } 
    else if (tipo == 2) { // Ejecutor / Asesino
        p.clase = "Ejecutor"; p.HP_Base = 100; p.HP_Base_Max = 100; p.dano_base = 30; p.habilidad = "Golpe Subito";
        apilarEscudo(p.escudos, "Anti-Plasma", 80); // Nace con 1 escudo
    } 
    else if (tipo == 3) { // Espectro / Hacker
        p.clase = "Espectro"; p.HP_Base = 60; p.HP_Base_Max = 60; p.dano_base = 10; p.habilidad = "Control Mental";
        p.escudos = nullptr; // Nace sin escudos
    } 
    else {
        p.clase = "Personalizado"; p.HP_Base = 100; p.HP_Base_Max = 100; p.dano_base = 20; p.habilidad = "Modificacion Libre";
        apilarEscudo(p.escudos, "Escudo Basico", 50);
    }
    
    generarSuministros(p.municiones, p.clase);
    return p;
}

/* ==========================================================
5. NODO DEL SISTEMA
==========================================================*/
struct NodoSistema {
    int ID_Clave;
    int Bando;
    string nombre_bando;
    Personaje operativo; 
    NodoSistema* siguiente = nullptr;
};

/*==========================================================
6. FUNCIONES AUXILIARES Y DE VALIDACIÓN
==========================================================*/
bool esNombreValido(const string& texto) {
    if (texto.empty()) return false;
    for (char c : texto) { if (isdigit(c)) return false; }
    return true;
}

bool existeID_Clave(NodoSistema* cabeza, int id_buscar) {
    NodoSistema* aux = cabeza;
    while (aux != nullptr) {
        if (aux->ID_Clave == id_buscar) return true;
        aux = aux->siguiente;
    }
    return false;
}

int leerEnteroSeguro() {
    int numero;
    while (true) {
        cin >> numero;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[Error] Entrada invalida. Ingrese solo valores numericos: ";
        } else { return numero; }
    }
}

string leerTextoSeguro(const string& mensaje) {
    string entrada;
    while (true) {
        cout << mensaje;
        getline(cin, entrada);
        if (esNombreValido(entrada)) return entrada;
        cout << "[Error] Entrada invalida. Intente de nuevo.\n";
    }
}

NodoSistema* buscarOperativoPorID_Clave(NodoSistema* cabeza, int id_buscar) {
    NodoSistema* aux = cabeza;
    while (aux != nullptr) {
        if (aux->ID_Clave == id_buscar) return aux;
        aux = aux->siguiente;
    }
    return nullptr;
}

void agregarItem(NodoItem* &inventario, string nombre, string tipoEfecto, int valor) {
    NodoItem* nuevo = new NodoItem{nombre, tipoEfecto, valor, inventario};
    inventario = nuevo;
}

void mostrarInventario(NodoItem* cabeza) {
    if (!cabeza) { cout << "Vacio"; return; }
    NodoItem* aux = cabeza;
    while (aux != nullptr) {
        cout << "[" << aux->nombre_item << " (+" << aux->valor_efecto << ")] ";
        aux = aux->siguiente;
    }
}

/*==========================================================
7. PROTOTIPO DE FUNCIÓN BORRAR
==========================================================*/
void borrar(NodoSistema* &cabeza, int id_buscar, NodoItem* &itemsCaidos);

/*==========================================================
8. INICIALIZACIÓN DE AVATARES HISTÓRICOS 
==========================================================*/
void cargarAvataresHistoricos(NodoSistema* &cabeza) {
    // 1. Borin "Corazón de Bronce"
    NodoSistema* borin = new NodoSistema;
    borin->ID_Clave = 10; borin->Bando = 1; borin->nombre_bando = "Resistencia Neon";
    borin->operativo.clase = "Tanque"; borin->operativo.HP_Base = 150; borin->operativo.HP_Base_Max = 150;
    borin->operativo.dano_base = 15; borin->operativo.habilidad = "Fuerza Bruta";
    apilarEscudo(borin->operativo.escudos, "Fisico", 50); apilarEscudo(borin->operativo.escudos, "Fisico", 50);
    encolarBlast(borin->operativo.municiones, 5, "Racimo (AoE)", 20);
    encolarBlast(borin->operativo.municiones, 5, "Racimo (AoE)", 20); // Nace con 2 Blasts Racimo
    borin->siguiente = cabeza; cabeza = borin; contadorNeon++;

    // 2. Dra. Emily Carter
    NodoSistema* carter = new NodoSistema;
    carter->ID_Clave = 20; carter->Bando = 1; carter->nombre_bando = "Resistencia Neon";
    carter->operativo.clase = "Espectro"; carter->operativo.HP_Base = 60; carter->operativo.HP_Base_Max = 60;
    carter->operativo.dano_base = 10; carter->operativo.habilidad = "Control Mental";
    carter->operativo.escudos = nullptr; // Sin escudos
    encolarBlast(carter->operativo.municiones, 3, "Troyano", 0);
    encolarBlast(carter->operativo.municiones, 3, "Troyano", 0);
    encolarBlast(carter->operativo.municiones, 3, "Troyano", 0); // Nace con 3 Troyanos
    carter->siguiente = cabeza; cabeza = carter; contadorNeon++;

    // 3. El Alfa / Paciente Cero
    NodoSistema* alfa = new NodoSistema;
    alfa->ID_Clave = 880; alfa->Bando = 2; alfa->nombre_bando = "Corporacion OMEGA";
    alfa->operativo.clase = "Tanque"; alfa->operativo.HP_Base = 150; alfa->operativo.HP_Base_Max = 150;
    alfa->operativo.dano_base = 15; alfa->operativo.habilidad = "Fuerza Bruta";
    apilarEscudo(alfa->operativo.escudos, "Escudo Espejo", 100);
    encolarBlast(alfa->operativo.municiones, 5, "Racimo (AoE)", 20);
    encolarBlast(alfa->operativo.municiones, 5, "Racimo (AoE)", 20);
    encolarBlast(alfa->operativo.municiones, 5, "Racimo (AoE)", 20);
    alfa->siguiente = cabeza; cabeza = alfa; contadorOmega++;

    // 4. Gnashrak "El Asediador"
    NodoSistema* gnashrak = new NodoSistema;
    gnashrak->ID_Clave = 990; gnashrak->Bando = 2; gnashrak->nombre_bando = "Corporacion OMEGA";
    gnashrak->operativo.clase = "Ejecutor"; gnashrak->operativo.HP_Base = 100; gnashrak->operativo.HP_Base_Max = 100;
    gnashrak->operativo.dano_base = 30; gnashrak->operativo.habilidad = "Golpe Subito";
    apilarEscudo(gnashrak->operativo.escudos, "Anti-Plasma", 80);
    encolarBlast(gnashrak->operativo.municiones, 2, "Perforante (EMP)", 50);
    encolarBlast(gnashrak->operativo.municiones, 2, "Perforante (EMP)", 50);
    encolarBlast(gnashrak->operativo.municiones, 4, "Disrupcion", 0);
    gnashrak->siguiente = cabeza; cabeza = gnashrak; contadorOmega++;
}

/*==========================================================
9. LÓGICA CON PILAS Y COLAS (COMBATE)
==========================================================*/
void ejecutarDisparo(NodoSistema* &cabeza, NodoItem* &itemsCaidos) {
    cout << "\n--- BATALLA: CONSUMO DE PILAS Y COLAS ---\n";
    cout << "Ingrese ID_Clave del Operativo Atacante: ";
    int idAtacante = leerEnteroSeguro();
    
    NodoSistema* atacante = buscarOperativoPorID_Clave(cabeza, idAtacante);
    if (!atacante || atacante->operativo.HP_Base <= 0) { cout << "[ERROR] Atacante no valido.\n"; return; }
    
    cout << "Ingrese ID_Clave del Operativo Objetivo (Enemigo): ";
    int idObjetivo = leerEnteroSeguro();
    if (idAtacante == idObjetivo) { cout << "[Aviso] No puedes atacarte a ti mismo.\n"; return; }
    
    NodoSistema* objetivo = buscarOperativoPorID_Clave(cabeza, idObjetivo);
    if (!objetivo || objetivo->Bando == atacante->Bando) { cout << "[ERROR] Objetivo no valido o aliado.\n"; return; }
    if (objetivo->operativo.HP_Base <= 0) { cout << "[AVISO] Objetivo ya destruido.\n"; return; }
    
    int danoProvocado = 0;
    string nombreAtaque = "";

    // Lógica Queue FIFO: Usar munición especial si tiene, si no, usa arma base.
    if (atacante->operativo.municiones.frente != nullptr) {
        danoProvocado = atacante->operativo.municiones.frente->dano;
        nombreAtaque = atacante->operativo.municiones.frente->tipo_blast;
        desencolarBlast(atacante->operativo.municiones); // Se consume el blast
    } else {
        danoProvocado = atacante->operativo.dano_base;
        nombreAtaque = "Arma Base Cuerpo a Cuerpo";
    }
    
    cout << "\n[¡PUM!] " << atacante->operativo.clase << " ataca con '" << nombreAtaque << "' infligiendo " << danoProvocado << " de dano.\n";
    
    // Lógica Stack LIFO: El daño choca con los escudos superiores primero
    while (danoProvocado > 0 && objetivo->operativo.escudos != nullptr) {
        NodoDefensa* tope = objetivo->operativo.escudos;
        if (tope->puntos_defensa >= danoProvocado) {
            tope->puntos_defensa -= danoProvocado;
            danoProvocado = 0;
            cout << "-> El escudo '" << tope->tipo_escudo << "' absorbio el impacto. Le quedan " << tope->puntos_defensa << " puntos.\n";
            if (tope->puntos_defensa == 0) desapilarEscudo(objetivo->operativo.escudos); // Escudo roto
        } else {
            danoProvocado -= tope->puntos_defensa;
            cout << "-> ¡CRITICO! El escudo '" << tope->tipo_escudo << "' ha sido destruido.\n";
            desapilarEscudo(objetivo->operativo.escudos);
        }
    }
    
    // Si sigue habiendo daño y no hay escudos, afecta HP_Base
    if (danoProvocado > 0) {
        objetivo->operativo.HP_Base -= danoProvocado;
        if (objetivo->operativo.HP_Base < 0) objetivo->operativo.HP_Base = 0;
        cout << "-> Daño directo a la estructura: Restan " << objetivo->operativo.HP_Base << " HP_Base.\n";
    }
    
    if (objetivo->operativo.HP_Base <= 0) {
        cout << "\n[¡ELIMINACIÓN CRÍTICA!] El operativo ID_Clave " << idObjetivo << " ha caido en batalla (Underflow).\n";
        borrar(cabeza, idObjetivo, itemsCaidos);
    }
}

/*==========================================================
10. MECÁNICA: HABILIDADES 
==========================================================*/
void ejecutarHabilidad(NodoSistema* &cabeza, NodoItem* &itemsCaidos) {
    cout << "\n--- ACTIVACION DE HABILIDAD ESPECIAL ---\n";
    cout << "Ingrese ID_Clave del Operativo que usara su habilidad: ";
    int idOp = leerEnteroSeguro();
    
    NodoSistema* ejecutor = buscarOperativoPorID_Clave(cabeza, idOp);
    if (!ejecutor) { cout << "[Error] El operativo no existe en la red.\n"; return; }
    if (ejecutor->operativo.HP_Base <= 0) { cout << "[Error] Un operativo desmantelado no puede usar habilidades.\n"; return; }

    string clase = ejecutor->operativo.clase;
    string hab = ejecutor->operativo.habilidad;
    cout << "-> Operativo de Clase '" << clase << "' activando: [" << hab << "]\n";

    if (hab == "Fuerza Bruta" || clase == "Juggernaut") {
        apilarEscudo(ejecutor->operativo.escudos, "Sobrecarga Bruta", 50);
        cout << "[!] Escudo de plasma apilado. Nueva capa generada con éxito.\n";
    }
    else if (hab == "Control Mental" || clase == "Espectro") {
        cout << "Ingrese ID_Clave del Objetivo Enemigo para robar datos (balas): ";
        int idObj = leerEnteroSeguro();
        NodoSistema* objetivo = buscarOperativoPorID_Clave(cabeza, idObj);
        
        if (!objetivo || objetivo->Bando == ejecutor->Bando || idObj == idOp) {
            cout << "[Aviso] Objetivo invalido o es un aliado.\n"; return;
        }
        
        if (objetivo->operativo.municiones.frente != nullptr) {
            string tipoRobado = objetivo->operativo.municiones.frente->tipo_blast;
            int danoRobado = objetivo->operativo.municiones.frente->dano;
            int idRobado = objetivo->operativo.municiones.frente->id_blast;
            
            desencolarBlast(objetivo->operativo.municiones);
            encolarBlast(ejecutor->operativo.municiones, idRobado, tipoRobado, danoRobado);
            cout << "[!] Exito. Has hackeado la cola del rival, extrayendo el blast '" << tipoRobado << "'.\n";
        } else {
            cout << "[Aviso] El rival no tiene municiones encoladas para robar.\n";
        }
    }
    else if (hab == "Golpe Subito" || clase == "Ejecutor") {
        cout << "Ingrese ID_Clave del Objetivo Enemigo a ejecutar: ";
        int idObj = leerEnteroSeguro();
        NodoSistema* objetivo = buscarOperativoPorID_Clave(cabeza, idObj);
        
        if (!objetivo || objetivo->Bando == ejecutor->Bando || idObj == idOp) {
            cout << "[Aviso] No puedes atacar a tus aliados.\n"; return;
        }
        if (objetivo->operativo.HP_Base <= 0) { cout << "[Aviso] El objetivo ya esta destruido.\n"; return; }
        
        objetivo->operativo.HP_Base -= 40; 
        if (objetivo->operativo.HP_Base < 0) objetivo->operativo.HP_Base = 0;
        cout << "[!] ¡FILTRACIÓN CRÍTICA! Golpe sorpresa inflige 40 de daño evadiendo sus defensas de escudo.\n";
        cout << ">> Estatus del Rival (ID_Clave: " << idObj << ") -> HP_Base: " << objetivo->operativo.HP_Base << "\n";
        
        if (objetivo->operativo.HP_Base <= 0) {
            cout << "\n[¡ELIMINACIÓN CRÍTICA!] El golpe subito destruyo al operativo ID_Clave " << idObj << ".\n";
            borrar(cabeza, idObj, itemsCaidos);
        }
    }
    else {
        cout << "[Aviso] Esta clase no tiene un modulo de accion programado o es de Modificacion Libre.\n";
    }
}

/*==========================================================
11. GESTIÓN CRUD 
==========================================================*/
void crear(NodoSistema* &cabeza, NodoItem* &itemsCaidos) {
    int tipo, bando_sel, id_p;
    cout << "\n--- REGISTRO DE OPERATIVO (INYECCIÓN) ---" << endl;
    cout << "Bando (1: Neon | 2: OMEGA): "; bando_sel = leerEnteroSeguro();
    
    if ((contadorNeon + contadorOmega) >= MAX_CAPACIDAD_GLOBAL) { 
        cout << "[ACCESO DENEGADO] Servidor Yggdrasil LLENO (" << (contadorNeon + contadorOmega) << "/" << MAX_CAPACIDAD_GLOBAL << "). No se permiten mas inyecciones en la red.\n";
        return;
    }
    
    cout << "Clase (1: Juggernaut | 2: Ejecutor | 3: Espectro | 4: PERSONALIZADO): "; 
    tipo = leerEnteroSeguro();
    
    while (true) {
        cout << "ID_Clave Unica del Operativo: "; id_p = leerEnteroSeguro();
        if (id_p == 10 || id_p == 20 || id_p == 880 || id_p == 990) cout << "[Error] ID Reservada de Avatar.\n";
        else if (existeID_Clave(cabeza, id_p)) cout << "[Error] ID ya registrada.\n";
        else break;
    }

    NodoSistema* nuevo = new NodoSistema;
    nuevo->ID_Clave = id_p;

    if (bando_sel == 1) { nuevo->Bando = 1; nuevo->nombre_bando = "Resistencia Neon"; contadorNeon++; } 
    else { nuevo->Bando = 2; nuevo->nombre_bando = "Corporacion OMEGA"; contadorOmega++; }

    cin.ignore(); 
    if (tipo == 4) {
        nuevo->operativo.clase = leerTextoSeguro("Nombre de la Clase/Rol: ");
        cout << "HP_Base: "; nuevo->operativo.HP_Base = leerEnteroSeguro();
        nuevo->operativo.HP_Base_Max = nuevo->operativo.HP_Base;
        cout << "Dano Base: "; nuevo->operativo.dano_base = leerEnteroSeguro();
        
        cout << "Cantidad de escudos iniciales a apilar: ";
        int cantEscudos = leerEnteroSeguro();
        for(int i=0; i<cantEscudos; i++) apilarEscudo(nuevo->operativo.escudos, "Custom", 50);
        
        cin.ignore();
        nuevo->operativo.habilidad = leerTextoSeguro("Habilidad Especial: ");
        generarSuministros(nuevo->operativo.municiones, "Custom");
    } else {
        nuevo->operativo = obtenerPlantillaPersonaje(tipo);
    }

    if (itemsCaidos != nullptr) {
        cout << "Hay items tirados en el suelo. ¿Desea recoger el ultimo? (1: Si | 2: No): ";
        if (leerEnteroSeguro() == 1) {
            NodoItem* rescatado = itemsCaidos;
            itemsCaidos = itemsCaidos->siguiente;
            rescatado->siguiente = nuevo->operativo.inventario;
            nuevo->operativo.inventario = rescatado;
            cout << "-> Item recogido.\n";
        }
    }

    nuevo->siguiente = cabeza; cabeza = nuevo;
    cout << "-> Operativo inyectado.\n";
}

void leer(NodoSistema* cabeza) {
    cout << "\n=== STATUS DE LOS SERVIDORES ===\n";
    if (!cabeza) { cout << "No hay operativos en red.\n"; return; }
    NodoSistema* aux = cabeza;
    while (aux != nullptr) {
        cout << "[ID: " << aux->ID_Clave << " | " << aux->nombre_bando << " | " << aux->operativo.clase 
             << " | HP_Base: " << aux->operativo.HP_Base << " | DanoBase: " << aux->operativo.dano_base;
        
        // Revisar tope de pila
        if (aux->operativo.escudos != nullptr) cout << " | Tope Escudo: " << aux->operativo.escudos->puntos_defensa;
        else cout << " | Escudos: 0";
        
        // Revisar frente de cola
        if (aux->operativo.municiones.frente != nullptr) cout << " | Siguiente Blast: " << aux->operativo.municiones.frente->tipo_blast;
        else cout << " | Municion: Vacia";
        
        cout << "]" << endl;
        aux = aux->siguiente;
    }
}

void actualizar(NodoSistema* cabeza, int id_buscar) {
    if (id_buscar == 10 || id_buscar == 20 || id_buscar == 880 || id_buscar == 990) {
        cout << "-> Acceso denegado: Los Avatares Historicos no pueden ser alterados.\n"; return;
    }
    NodoSistema* aux = cabeza;
    while (aux != nullptr) {
        if (aux->ID_Clave == id_buscar) {
            cout << "Ingrese nueva Vida (HP_Base): "; 
            aux->operativo.HP_Base = leerEnteroSeguro();
            cout << "¿Desea apilar un nuevo escudo reparado? (1: Si | 2: No): ";
            if (leerEnteroSeguro() == 1) apilarEscudo(aux->operativo.escudos, "Reparado", 50);
            cout << "-> Datos actualizados.\n";
            return;
        }
        aux = aux->siguiente;
    }
    cout << "-> ID_Clave no localizada.\n";
}

void vaciarPila(PilaDefensa &pila) {
    while (pila != nullptr) desapilarEscudo(pila);
}
void vaciarCola(ColaArsenal &cola) {
    while (cola.frente != nullptr) desencolarBlast(cola);
}

void borrar(NodoSistema* &cabeza, int id_buscar, NodoItem* &itemsCaidos) {
    if (id_buscar == 10 || id_buscar == 20 || id_buscar == 880 || id_buscar == 990) {
        cout << "-> Error: No puedes eliminar a los Avatares.\n"; return;
    }
    NodoSistema* aux = cabeza; NodoSistema* anterior = nullptr;
    while (aux != nullptr && aux->ID_Clave != id_buscar) { anterior = aux; aux = aux->siguiente; }
    if (!aux) { cout << "No se encontro el ID_Clave.\n"; return; }

    if (aux->Bando == 1) contadorNeon--; else contadorOmega--;
    if (!anterior) cabeza = cabeza->siguiente; else anterior->siguiente = aux->siguiente;

    NodoItem* itemAux = aux->operativo.inventario;
    while (itemAux != nullptr) {
        NodoItem* sigItem = itemAux->siguiente;
        itemAux->siguiente = itemsCaidos;
        itemsCaidos = itemAux;
        itemAux = sigItem;
    }

    // Limpiar memoria de TDA locales
    vaciarPila(aux->operativo.escudos);
    vaciarCola(aux->operativo.municiones);

    delete aux; 
    cout << "-> Operativo eliminado. Memoria de Colas y Pilas liberada.\n";
}

void borrar_todo(NodoSistema* &inicio) {
    while (inicio != nullptr) {
        NodoSistema* aux = inicio;
        inicio = inicio->siguiente;
        vaciarPila(aux->operativo.escudos);
        vaciarCola(aux->operativo.municiones);
        
        NodoItem* iAux = aux->operativo.inventario;
        while (iAux != nullptr) {
            NodoItem* iSig = iAux->siguiente;
            delete iAux; iAux = iSig;
        }
        delete aux; 
    }
    contadorNeon = 0; contadorOmega = 0;
}

void ejecutarUsoItemVida(NodoSistema* cabeza) {
    cout << "\n--- PROTOCOLO DE REGENERACIÓN (ITEMS) ---\n";
    cout << "Ingrese ID_Clave del Operativo: "; int idOp = leerEnteroSeguro();
    
    NodoSistema* op = buscarOperativoPorID_Clave(cabeza, idOp);
    if (!op || op->operativo.HP_Base <= 0 || !op->operativo.inventario) { cout << "[Error] Condicion invalida.\n"; return; }

    NodoItem* actual = op->operativo.inventario; NodoItem* anterior = nullptr;
    op->operativo.HP_Base += actual->valor_efecto;
    if (op->operativo.HP_Base > op->operativo.HP_Base_Max) op->operativo.HP_Base = op->operativo.HP_Base_Max; 
    cout << ">> Nueva integridad -> HP_Base: " << op->operativo.HP_Base << "\n";

    if (!anterior) op->operativo.inventario = actual->siguiente; 
    else anterior->siguiente = actual->siguiente;      
    delete actual; 
}


/*========================================================
12. MENÚ PRINCIPAL 
==========================================================*/
int main() {
    srand(time(0)); // Semilla para Blasts aleatorios
    NodoSistema* yggdrasil = nullptr;
    NodoItem* sueloItemsCaidos = nullptr; 
    int opcion = 0, id_busqueda;

    cargarAvataresHistoricos(yggdrasil);

    while (opcion != 9) {
        cout << "\n--- NUCLEO YGGDRAZIL (PILA Y COLA INTEGRADAS) ---" << endl;
        cout << "1. Inyectar Operativo" << endl;
        cout << "2. Ver Operativos" << endl;
        cout << "3. Actualizar HP_Base / Apilar Escudo" << endl;
        cout << "4. Eliminar Operativo Manualmente" << endl;
        cout << "5. Reiniciar Sistema" << endl;
        cout << "6. Ver Items Tirados en el Suelo" << endl;
        cout << "7. [BATALLA] SIMULAR DISPARO" << endl;
        cout << "8. [MECANICA] USAR HABILIDAD ESPECIAL DE LA CLASE" << endl;
        cout << "9. Salir" << endl;
        cout << "Seleccion: "; opcion = leerEnteroSeguro();

        if (opcion == 1) crear(yggdrasil, sueloItemsCaidos);
        else if (opcion == 2) leer(yggdrasil); 
        else if (opcion == 3) { cout << "ID_Clave a actualizar: "; id_busqueda = leerEnteroSeguro(); actualizar(yggdrasil, id_busqueda); } 
        else if (opcion == 4) { cout << "ID_Clave a borrar: "; id_busqueda = leerEnteroSeguro(); borrar(yggdrasil, id_busqueda, sueloItemsCaidos); } 
        else if (opcion == 5) { borrar_todo(yggdrasil); cargarAvataresHistoricos(yggdrasil); cout << "-> Sistema Reiniciado.\n"; } 
        else if (opcion == 6) mostrarInventario(sueloItemsCaidos);
        else if (opcion == 7) ejecutarDisparo(yggdrasil, sueloItemsCaidos);
        else if (opcion == 8) ejecutarHabilidad(yggdrasil, sueloItemsCaidos);
    } 

    borrar_todo(yggdrasil);
    return 0;
}
