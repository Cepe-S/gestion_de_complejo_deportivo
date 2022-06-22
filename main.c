#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <conio.h>

// borra todo lo que está en el stdout
void clrscr() {
    system("@cls||clear");
}

// TODO: mover funciones a librerias

typedef struct {
    char tipo; // f5: futbol, b: basket, t: tenis, p: padel, s: squash
    int precio; // por 2 horas
    int capacidad;
} tipo_cancha;

typedef struct {
    int id;
    char tipo;
} cancha;

typedef struct {
    char apellido[20];
    int dni; // id único
    int telefono;
} cliente;

// un turno = 2 horas
typedef struct {
    int id_turno; // 1 = 10 a 12, 2 = 12 a 14, 3 = 14 a 16, 4 = 16 a 18, 5 = 18 a 20, 6 = 20 a 22
    int dia;
    int mes;
    int cancha_id;
    int cliente_id;
    int baja;
} turno;

// loads an tipo_cancha struct from console and returns it
tipo_cancha cargar_tipo_cancha() {
    tipo_cancha t;
    printf("Ingrese el tipo de cancha: ");
    fflush(stdin);
    scanf("%c", &t.tipo);
    printf("Ingrese el precio: ");
    scanf("%d", &t.precio);
    printf("Ingrese la capacidad: ");
    scanf("%d", &t.capacidad);
    return t;
}

// opens a file and saves canchas in an array
void cargar_canchas() {
    FILE *archivo;
    
    tipo_cancha c;
    for (int i = 0; i < 5; i++) {
        archivo = fopen("canchas.dat", "ab");
        fflush(stdin);
        c = cargar_tipo_cancha();
        fwrite(&c, sizeof(tipo_cancha), 1, archivo);
        fclose(archivo);
    }
}

// prints the file canchas.dat in the console
void mostrar_canchas() {
    FILE *archivo;
    archivo = fopen("canchas.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    tipo_cancha c[size];
    fread(&c, sizeof(cancha), size, archivo);
    for (int i = 0; i < 5; i++) {
        printf("Tipo de cancha: %c\n", c[i].tipo);
        printf("Precio de la cancha: %d\n", c[i].precio);
        printf("Capacidad de la cancha: %d\n", c[i].capacidad);
    }
    fclose(archivo);
}

// loads a cancha struct from console and returns it
cancha cargar_cancha() {
    cancha c;
    printf("Ingrese el tipo de cancha: ");
    fflush(stdin);
    scanf("%c", &c.tipo);
    printf("Ingrese el id de la cancha: ");
    scanf("%d", &c.id);
    printf("Ingrese el estado de la cancha: ");
    fflush(stdin);
    return c;
}

// MODULARIZAR

// cargar canchas en un archivo

// carga clientes solo cuando no estan en el archivo
cliente crear_cliente() {
    cliente c;
    printf("Ingrese el dni del cliente: ");
    scanf("%d", &c.dni);
    // agregar función que revise si el cliente ya esta en el archivo
    printf("Ingrese el apellido del cliente: ");
    fflush(stdin);
    scanf("%s", &c.apellido);
    fflush(stdin);
    printf("Ingrese el telefono del cliente: ");
    scanf("%d", &c.telefono);
    return c;
}

void print_cliente(cliente c) {
    printf("Apellido: %s\n", c.apellido);
    printf("DNI: %d\n", c.dni);
    printf("Telefono: %d\n", c.telefono);
}

// carga un cliente pasado por parametro al archivo
void cargar_cliente() {
    int i;
    bool run = true;
    cliente c;
    while (run) {
        clrscr();
        c = crear_cliente();
        clrscr();
        printf("--- Cliente creado ---\n\n");
        print_cliente(c);
        printf("\nPresione enter para confirmar\n");
        if (getch() == 13) {
            run = false;
        }
    }
    FILE *archivo;
    archivo = fopen("clientes.dat", "ab");
    fwrite(&c, sizeof(cliente), 1, archivo);
    fclose(archivo);
    printf("Cliente agregado con exito\n");
    sleep(1);
    clrscr();
}

// muestra los clientes en formato de tabla
void mostrar_clientes() {
    FILE *archivo;
    archivo = fopen("clientes.dat", "rb");
    fseek(archivo, 0L, SEEK_END);
    int size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    cliente c[size];
    fread(&c, sizeof(cliente), size, archivo);
    for (int i = 0; i < size / sizeof(cliente); i++) {
        print_cliente(c[i]);
        printf("------------------------------\n");
    }
    fclose(archivo);
}
// carga turnos solo cuando el horario no esta ocupado
void cargar_turnos() {
    FILE *archivo;
    archivo = fopen("turnos.dat", "ab");
    fflush(stdin);
    turno t;
    for (int i = 0; i < 5; i++) {
        printf("Ingrese el id del turno: ");
        scanf("%d", &t.id_turno);
        printf("Ingrese el dia del turno: ");
        scanf("%d", &t.dia);
        printf("Ingrese el mes del turno: ");
        scanf("%d", &t.mes);
        printf("Ingrese el id de la cancha: ");
        scanf("%d", &t.cancha_id);
        printf("Ingrese el id del cliente: ");
        scanf("%d", &t.cliente_id);
        fwrite(&t, sizeof(turno), 1, archivo);
    }
    fclose(archivo);
}


// valida que el turno no este ocupado buscando en el archivo turnos.dat

// da de baja el turno


void bajaTurno(char NombreArchivo,int idcliente)
{
    FILE*archivoTurno =fopen(NombreArchivo,"rb");
    turno turn;
    int flag=0;
    
    if(archivoTurno!=NULL)
    {
        while(fread(&turn,sizeof(turno),1,archivoTurno)>0 && flag==0)
        {
            if(idcliente ==turn.cliente_id)
            {
                mostrar_clientes();
                turn.baja=0;
                flag=1;
            }
        }
        fclose(archivoTurno);
    }
}

// muestra los turnos de un dia especifico enviado por parametro

// validar dias, meses y turnos

// hacer el menu

// traba el sistema hasta que se aprete enter


//MODIFICA TURNO

void modificarTurno(char NombreArchivo[30], int pos)
{
    FILE *archivoTurn = fopen("NombreArchivo","rb);
    turno turnocancha;

    if(archivoTurn!=NULL)
    {
        fseek(archivoAlu,sizeof(turno)*(pos-1),0);
        fread(&turnocancha,sizeof(turno),1,archivoAlu);

        mostrar_canchas();

        fclose(archivoTurn);
    }
}
                              
//BUSQUEDA
                              
void buscarTurno(char NombreArchivo[30], int pos)
{
    FILE *archivoTurno = fopen("NombreArchivo","rb");

    turno turn;

    if(archivoTurn!=NULL)
    {
        fseek(archivoTurno,sizeof(turno)*(pos-1),0);
        fread(&alu,sizeof(turno),1,archivoTurno);
        mostrar_canchas();
        fclose(archivoTurno);
    }
    
}

void volver() {
    bool run = true;
    printf("--> Volver");
    while (run) {
        if (getch() == 13) {
            run = false;
            clrscr();
        }
    }
}

void print_menu(int option, int menu_number) {
    if (menu_number == 0) {  // menu principal
            switch (option) {
                case 1: 
                printf("--> Menu clientes\n");
                break;
                default:
                printf("Menu clientes\n");
            }
            switch (option) {
                case 2:
                printf("--> Menu turnos\n");
                break;
                default:
                printf("Menu turnos\n");
            }
            switch (option) {
                case 3:
                printf("--> ---------\n");
                break;
                default:
                printf("---------\n");
            }
            switch (option) {
                case 4:
                printf("--> ---------\n");
                break;
                default:
                printf("---------\n");
            }
            switch (option) {
                case 5:
                printf("--> ---------\n");
                break;
                default:
                printf("---------\n");
            }
            switch (option) {
                case 6:
                printf("--> Salir\n");
                break;
                default:
                printf("Salir\n");
            }
    }
    if (menu_number == 1) {  // menu clientes
        switch (option) {
            case 1:
            printf("--> Agregar cliente\n");
            break;
            default:
            printf("Agregar cliente\n");
        }
        switch (option) {
            case 2:
            printf("--> Mostrar clientes\n");
            break;
            default:
            printf("Mostrar clientes\n");
        }
        switch (option) {
            case 3:
            printf("--> ---------\n");
            break;
            default:
            printf("---------\n");
        }    
        switch (option) {
            case 4:
            printf("--> ---------\n");
            break;
            default:
            printf("---------\n");
        }
        switch (option) {
            case 5:
            printf("--> ---------\n");
            break;
            default:
            printf("---------\n");
        }
            switch (option) {
            case 6:
            printf("--> Volver\n");
            break;
            default:
            printf("Volver\n");
            break;
        }
    }
    if (menu_number == 2) {  // menu clientes
        switch (option) {
            case 1:
            printf("--> Agregar turno\n");
            break;
            default:
            printf("Agregar turno\n");
        }
        switch (option) {
            case 2:
            printf("--> Mostrar turnos\n");
            break;
            default:
            printf("Mostrar turnos\n");
        }
        switch (option) {
            case 3:
            printf("--> ---------\n");
            break;
            default:
            printf("---------\n");
        }    
        switch (option) {
            case 4:
            printf("--> ---------\n");
            break;
            default:
            printf("---------\n");
        }
        switch (option) {
            case 5:
            printf("--> ---------\n");
            break;
            default:
            printf("---------\n");
        }
            switch (option) {
            case 6:
            printf("--> Volver\n");
            break;
            default:
            printf("Volver\n");
            break;
        }
    }
}

// organiza las funciones de los menús
void ejecutador(int menu_number, int function_number) {
    if (menu_number == 1) {  // funciones del menu 1, clientes
        switch (function_number) {
            case 1:
                cargar_cliente();
                break;
            case 2:
                mostrar_clientes();
                volver();
                break;
            default:
                break;
        }
    }
}

void menu(int menu_number) {
    bool run = true;
    int i = 1;
    while (run) {
        print_menu(i, menu_number);
        int c = getch();
        clrscr();
        switch (c) {
        case 72:
            if (i > 1) {i--;}
            break;
        case 80:
            if (i < 6) {i++;}
            break;
        case 48:
            run = false;
            break;
        case 13:
            switch (i) {
                case 1:
                ejecutador(menu_number, 1);
                break;
                case 2:
                ejecutador(menu_number, 2);
                break;
                case 3:
                // funcion 3
                break;
                case 4:
                // funcion 4
                break;
                case 5:
                run = false;
                break;
                case 6:
                run = false;
                break;
            }
        }
    }
}

void main_menu() {
    bool run = true;
    int i = 1;
    while (run) {
        print_menu(i, 0);
        int c = getch();
        clrscr();
        switch (c) {
        case 72:
            if (i > 1) {i--;}
            break;
        case 80:
            if (i < 6) {i++;}
            break;
        case 48:
            run = false;
            break;
        case 13:
            switch (i) {
                case 1:
                menu(1); // abre el menu clientes
                break;
                case 2:
                menu(2); // abre el menu turnos
                break;
                case 3:
                // funcion 3
                break;
                case 4:
                // funcion 4
                break;
                case 5:
                // funcion 5
                break;
                case 6:
                run = false;
                break;
            }
        }
    }
}

int main() {
    int menu_number = 1;
    main_menu();
    return 0;
}
