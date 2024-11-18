-- Crear la base de datos BBDD_PARCHIS
	CREATE DATABASE IF NOT EXISTS BBDD_PARCHIS;

-- Usar la base de datos
	USE BBDD_PARCHIS;

-- Crear la tabla partida
	CREATE TABLE IF NOT EXISTS partida (
										ID_Partida VARCHAR(10) NOT NULL PRIMARY KEY,  -- Clave primaria, identificador único para cada partida
										fecha DATE,                                  -- Fecha de la partida
										N_Jugadores INT,                             -- Número de jugadores en la partida
										Jugador1 VARCHAR(50),                        -- Nombre del Jugador 1
										Jugador2 VARCHAR(50),                       -- Nombre del Jugador 2
										Jugador3 VARCHAR(50),                        -- Nombre del Jugador 3
										Jugador4 VARCHAR(50)                         -- Nombre del Jugador 4
										);

-- Crear la tabla usuario
	CREATE TABLE IF NOT EXISTS usuario (
										ID_Usuario VARCHAR(50) NOT NULL PRIMARY KEY,  -- Clave primaria, identificador único para cada usuario
										Nombre_Usuario VARCHAR(50),                   -- Nombre del usuario
										mail VARCHAR(100) UNIQUE,                     -- Correo del usuario, debe ser único
										Password VARCHAR(50),                           -- Contraseña del usuario
										Victorias INT DEFAULT 0                       -- Número de victorias del usuario, valor por defecto es 0
										);


-- Crear la tabla premios
	CREATE TABLE IF NOT EXISTS premios (
										PREMIO_Buscado VARCHAR(50) NOT NULL PRIMARY KEY,  -- Clave primaria, identificador único para cada tipo de premio buscado
										Premio1 VARCHAR(50),                              -- Primer premio
										Premio2 VARCHAR(50),                               -- Segundo premio
										Premio3 VARCHAR(50),                              -- Tercer premio
										Premio4 VARCHAR(50)                               -- Cuarto premio
										);





-- Insertar valores de ejemplo en la tabla partida
	INSERT INTO partida (ID_Partida, fecha, N_Jugadores, Jugador1, Jugador2, Jugador3, Jugador4)
	VALUES
	('P001', '2024-10-01', 4, 'Sergi', 'Yerald', 'Nizar', 'Alex'),
	('P002', '2024-10-02', 3, 'Yerald', 'Sergi', 'Nizar', NULL),
	('P003', '2024-10-03', 2, 'Yerald', 'Nizar', NULL, NULL);



-- Insertar algunos datos de ejemplo en la tabla usuario
	INSERT INTO usuario (ID_Usuario, Nombre_Usuario, mail, Password, Victorias)
	VALUES
	('user1', 'Sergi', 'maria@example.com', 'password123', 5),
	('user2', 'Yerald', 'sergio@example.com', 'password456', 10),
	('user3', 'Nizar', 'ana@example.com', 'password789', 3);

-- Insertar valores de ejemplo en la tabla premios
	INSERT INTO premios (PREMIO_Buscado, Premio1, Premio2, Premio3, Premio4)
	VALUES
	('Medalla de Plata', 'Medalla de Bronce', 'Medalla de Plata', 'Medalla de Oro', 'Medalla Diamante'),
	('Medalla de Oro', 'Medalla de Bronce', 'Medalla de Plata', 'Medalla de Oro', 'Medalla Diamante'),
	('Medalla Diamante', 'Medalla de Bronce', 'Medalla de Plata', 'Medalla de Oro', 'Medalla Diamante');

