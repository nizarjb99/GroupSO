#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include <pthread.h>

MYSQL* conectar_bd();
void consultar_nombre(char* id_usuario, char* respuesta);
void consultar_mail(char* id_usuario, char* respuesta);
void consultar_password(char* id_usuario, char* respuesta);

typedef struct {
	char nombre[20];
	int socket;        // Socket del jugador
	  // Nombre del jugador
} Conectado;

typedef struct{
	Conectado conectados[100];
	int num;
}ListaConectados;

ListaConectados lista;

typedef struct {
	int total_invitados;
	int respuestas_recibidas;
	int aceptados;
	int socket_invitador;
} Invitacion;

Invitacion invitacion_actual;

// Funcion para enviar un mensaje a todos los clientes conectados
void enviar_a_todos(const char *mensaje) {
	for (int i = 0; i < lista.num; i++) {
		write(lista.conectados[i].socket, mensaje, strlen(mensaje));
	}
}

// Funcion para notificar automÃ¡ticamente la lista de usuarios conectados
void notificar_lista() {
	char mensaje[1024];
	if (lista.num == 0) {
		strcpy(mensaje, "No hay jugadores conectados.\n");
	} else {
		strcpy(mensaje, "Jugadores conectados:\n");
		for (int i = 0; i < lista.num; i++) {
			strcat(mensaje, lista.conectados[i].nombre);
			strcat(mensaje, "\n");
		}
	}
	enviar_a_todos(mensaje);
}

//Funcion para agregar un jugador a la lista
void agregar_jugador(int socket, const char *nombre) {
	if (lista.num < 100) {
		lista.conectados[lista.num].socket = socket;
		strcpy(lista.conectados[lista.num].nombre, nombre);
		lista.num++;
		notificar_lista();  // Notificamos a todos los clientes conectados
	}
}

// Funcion para eliminar un jugador de la lista
void eliminar_jugador(int socket) {
	for (int i = 0; i < lista.num; i++) {
		if (lista.conectados[i].socket == socket) {
			lista.conectados[i] = lista.conectados[lista.num - 1];
			lista.num--;
			notificar_lista();  // Notificamos a todos los clientes conectados
			break;
		}
	}
}

// Funcion para obtener la lista de jugadores conectados
void obtener_lista_jugadores(char *respuesta) {
	if (lista.num == 0) {
		strcpy(respuesta, "No hay jugadores conectados.");
	} else {
		strcpy(respuesta, "Jugadores conectados:\n");
		for (int i = 0; i < lista.num; i++) {
			strcat(respuesta, lista.conectados[i].nombre);
			strcat(respuesta, "\n");
		}
	}
}


void enviar_invitaciones(int socket_invitador, const char *nombre_invitador) {
	char mensaje[256];
	sprintf(mensaje, "INVITACION/%s te ha invitado a jugar. ¿Aceptas? (si/n0)\n", nombre_invitador);
	
	invitacion_actual.total_invitados = lista.num - 1; // Excluye al invitador
	invitacion_actual.respuestas_recibidas = 0;
	invitacion_actual.aceptados = 0;
	invitacion_actual.socket_invitador = socket_invitador;
	for (int i = 0; i < lista.num; i++) {
		if (lista.conectados[i].socket != socket_invitador) {
			write(lista.conectados[i].socket, mensaje, strlen(mensaje));
		}
	}
}
void procesar_respuesta_invitacion(int socket_invitador, const char *resultado) {
	invitacion_actual.respuestas_recibidas++;
	
	if (strcmp(resultado, "s") == 0) {
		invitacion_actual.aceptados++;
	}
	
	if (invitacion_actual.respuestas_recibidas == invitacion_actual.total_invitados) {
		char mensaje_general[256];
		char mensaje_invitador[256];
		
		// Mensaje para todos los jugadores
		if (invitacion_actual.aceptados == invitacion_actual.total_invitados) {
			sprintf(mensaje_general, "RESULTADO/¡Todos aceptaron! El juego puede empezar.\n");
		} else {
			sprintf(mensaje_general, "RESULTADO/No todos aceptaron. Decidan si desean jugar de todos modos.\n");
		}
		
		// Mensaje exclusivo para el invitador
		sprintf(
				mensaje_invitador,
				"RESUMEN/De los %d invitados, %d aceptaron y %d rechazaron.\n",
				invitacion_actual.total_invitados,
				invitacion_actual.aceptados,
				invitacion_actual.total_invitados - invitacion_actual.aceptados
				);
		
		// Informar al invitador especÃ­ficamente
		printf("Enviando resumen al socket invitador: %d\n", invitacion_actual.socket_invitador);
		printf("Mensaje: %s\n", mensaje_invitador);
		if (write(invitacion_actual.socket_invitador, mensaje_invitador, strlen(mensaje_invitador)) < 0) {
			perror("Error enviando mensaje al invitador");
		}
		
		// Informar a todos los jugadores conectados
		for (int i = 0; i < lista.num; i++) {
			if (lista.conectados[i].socket == invitacion_actual.socket_invitador) {
				write(lista.conectados[i].socket, mensaje_invitador, strlen(mensaje_invitador));
			}
			write(lista.conectados[i].socket, mensaje_general, strlen(mensaje_general));
		}
	}
}



int contador;

//Estructura necesaria para acceso excluyente
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *AtenderCliente(void *socket)
{
	int sock_conn;
	int *s;
	s= (int *) socket;
	sock_conn= *s;
	
	char peticion[512];
	char id_usuario[20];
	char respuesta[512];
	int ret;
	char buff[512];
	char buff2[512];
	char nombre_jugador[20];
	
	int terminar =0;
	//Entramos en bucle para atender todas las peticiones de este cliente hasta que se desconecte
	
	// ¡¡¡¡PROGRAMA REAL ELIMINADO POR FALLO EN AL BASE DE DATOS MYSQL!!!!
	//char nombre_jugador[20];
	//consultar_nombre(id_usuario, buff2);  // Esto llena el buffer 'buff2' con el nombre del jugador
	//strcpy(nombre_jugador, buff2);  // Ahora puedes copiar el nombre recibido al array nombre_jugador
	//agregar_jugador(sock_conn, nombre_jugador);  // Agregar el jugador con su socket y nombre
	// ¡¡¡¡PROGRAMA REAL ELIMINADO POR FALLO EN AL BASE DE DATOS MYSQL!!!!
	
	// Simulacion de un nombre fijo para el jugador
	
	strcpy(nombre_jugador, "Jugador"); // Nombre fijo para pruebas
	agregar_jugador(sock_conn, nombre_jugador);
	
	while (terminar ==0)
	{
		//Escribimos la peticon
		ret = read(sock_conn, peticion, sizeof(peticion));
		printf("Recibido\n");
		
		
		//Tenemos que añadirle la marca de fin de string
		//para que no escriba lo que hay despues en el buffer
		peticion[ret]='\0';
		
		printf("Peticion: %s\n",peticion);
		
		//vamos a ver que es lo que quiere
		char *p = strtok(peticion, "/");
		int codigo = atoi (p);
		
		//ya tenemos la peticion
		
		
		if (codigo == 0){//Peticion de desconexión
			eliminar_jugador(sock_conn);
			terminar=1;
		} 
		
		else if (codigo ==1) //piden que le digan el nombre
			consultar_nombre(id_usuario, buff2);
		else if (codigo ==2)
			consultar_mail(id_usuario, buff2);
		else if (codigo ==3)
			consultar_password(id_usuario, buff2);
		else if (codigo == 4) 
		{ // El codigo 4 sera usado para invitaciones
			enviar_invitaciones(sock_conn, nombre_jugador);
		} 
		else if (codigo == 5) 
		{ // El codigo 5 es para procesar respuesta de invitacion
			char *resultado = strtok(NULL, "/");
			procesar_respuesta_invitacion(sock_conn, resultado);
		}
		if (codigo !=0)
		{
			printf("Respuesta : %s\n", respuesta);
			//enviamos la presuesta
			write(sock_conn, respuesta, strlen(respuesta));
			
		}
		if ((codigo ==1) ||(codigo ==2) || (codigo ==3)|| (codigo ==4)|| (codigo ==5))
		{
			pthread_mutex_lock( &mutex); //No me interrumpas ahora
			contador = contador +1;
			pthread_mutex_unlock( &mutex); //Ya puedes interrumpir
		}
	}
	// se acabo el servicio para este cliente
	close(sock_conn);
}

int main(int argc, char *argv[])
{
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	
	lista.num=0; //Inicializacion de la lista
	
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9050
	serv_adr.sin_port = htons(9050);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(sock_listen, 2) < 0)
		printf("Error en el Listen");
	
	contador =0;
	int i;
	int sockets[100];
	pthread_t thread;
	i=0;
	
	
	for(;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		
		sockets[i] = sock_conn;
		//sock_conn es el socket que usaremos para este cliente
		
		//crear thead y decirle lo que tiene que hacer
		
		pthread_create (&thread, NULL, AtenderCliente, &sockets[i]);
		i=i+1;
	}
	//for(i=0; i<5;i++)
		//pthread_join (thread[i], NULL);
	
}


// Conexion a la base de datos
MYSQL* conectar_bd() {
	MYSQL *conn;
	conn = mysql_init(NULL);
	if (conn == NULL) {
		printf("Error al crear el objeto MySQL: %s\n", mysql_error(conn));
		exit(1);
	}
	
	// Conectar a la base de datos
	if (mysql_real_connect(conn, "localhost", "usuario", "password", "BBDD_PARCHIS", 0, NULL, 0) == NULL) {
		printf("Error al conectar a la base de datos: %s\n", mysql_error(conn));
		mysql_close(conn);
		exit(1);
	}
	
	return conn;
}

// Consultar nombre del usuario
void consultar_nombre(char* id_usuario, char* respuesta) {
	MYSQL *conn = conectar_bd();
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	char consulta[256];
	sprintf(consulta, "SELECT Nombre_Usuario FROM usuario WHERE ID_Usuario = '%s'", id_usuario);
	
	if (mysql_query(conn, consulta)) {
		printf("Error en la consulta: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en la consulta");
		mysql_close(conn);
		return;
	}
	res = mysql_store_result(conn);
	if (res == NULL) {
		printf("Error al obtener resultados: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en la consulta");
		mysql_close(conn);
		return;
	}
	
	if ((row = mysql_fetch_row(res))) {
		sprintf(respuesta, "Nombre: %s", row[0]);
	} else {
		strcpy(respuesta, "Usuario no encontrado");
	}
	
	mysql_free_result(res);
	mysql_close(conn);
}

// Consultar correo del usuario
void consultar_mail(char* id_usuario, char* respuesta) {
	MYSQL *conn = conectar_bd();
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	char consulta[256];
	sprintf(consulta, "SELECT mail FROM usuario WHERE ID_Usuario = '%s'", id_usuario);
	
	if (mysql_query(conn, consulta)) {
		printf("Error en la consulta: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en la consulta");
		mysql_close(conn);
		return;
	}
	
	res = mysql_store_result(conn);
	if (res == NULL) {
		printf("Error al obtener resultados: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en la consulta");
		mysql_close(conn);
		return;
	}
	
	if ((row = mysql_fetch_row(res))) {
		sprintf(respuesta, "Correo: %s", row[0]);
	} else {
		strcpy(respuesta, "Usuario no encontrado");
	}
	
	mysql_free_result(res);
	mysql_close(conn);
}

// Consultar contraseña del usuario
void consultar_password(char* id_usuario, char* respuesta) {
	MYSQL *conn = conectar_bd();
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	char consulta[256];
	sprintf(consulta, "SELECT Password FROM usuario WHERE ID_Usuario = '%s'", id_usuario);
	
	if (mysql_query(conn, consulta)) {
		printf("Error en la consulta: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en la consulta");
		mysql_close(conn);
		return;
	}
	
	res = mysql_store_result(conn);
	if (res == NULL) {
		printf("Error al obtener resultados: %s\n", mysql_error(conn));
		strcpy(respuesta, "Error en la consulta");
		mysql_close(conn);
		return;
	}
	
	if ((row = mysql_fetch_row(res))) {
		sprintf(respuesta, "Contraseña: %s", row[0]);
	} else {
		strcpy(respuesta, "Usuario no encontrado");
	}
	
	mysql_free_result(res);
	mysql_close(conn);
}



