-- Crear la base de datos BBDD_PARCHIS
CREATE DATABASE IF NOT EXISTS BBDD_PARCHIS;

-- Usar la base de datos
USE BBDD_PARCHIS;

-- Crear la tabla usuario
CREATE TABLE IF NOT EXISTS usuario (
    ID_Usuario INT NOT NULL AUTO_INCREMENT PRIMARY KEY,  -- ID autoincremental para cada usuario
    Nombre_Usuario VARCHAR(50) NOT NULL,                 -- Nombre del usuario
    mail VARCHAR(100) UNIQUE NOT NULL,                   -- Correo del usuario, debe ser único
    Password VARCHAR(256) NOT NULL,                      -- Contraseña del usuario (almacenar hash, no texto plano)
    Victorias INT DEFAULT 0                              -- Número de victorias del usuario, valor por defecto es 0
);

-- Crear la tabla partida
CREATE TABLE IF NOT EXISTS partida (
    ID_Partida INT NOT NULL AUTO_INCREMENT PRIMARY KEY,  -- Clave primaria, identificador único para cada partida
    fecha DATE,                                          -- Fecha de la partida
    N_Jugadores INT,                                     -- Número de jugadores en la partida
    Jugador1 INT,                                        -- ID del Jugador 1
    Jugador2 INT,                                        -- ID del Jugador 2
    Jugador3 INT,                                        -- ID del Jugador 3
    Jugador4 INT,                                        -- ID del Jugador 4
    FOREIGN KEY (Jugador1) REFERENCES usuario(ID_Usuario),  -- Clave foránea para asegurar la integridad
    FOREIGN KEY (Jugador2) REFERENCES usuario(ID_Usuario),
    FOREIGN KEY (Jugador3) REFERENCES usuario(ID_Usuario),
    FOREIGN KEY (Jugador4) REFERENCES usuario(ID_Usuario)
);

-- Crear la tabla premios
CREATE TABLE IF NOT EXISTS premio (
    ID_Premio INT NOT NULL AUTO_INCREMENT PRIMARY KEY,  -- Clave primaria para cada premio
    Nombre_Premio VARCHAR(50) NOT NULL                  -- Nombre del premio
);

-- Crear la tabla premio_usuario (relación de premios ganados por usuarios)
CREATE TABLE IF NOT EXISTS premio_usuario (
    ID_Usuario INT NOT NULL,
    ID_Premio INT NOT NULL,
    Fecha_Obtencion DATE,
    PRIMARY KEY (ID_Usuario, ID_Premio),
    FOREIGN KEY (ID_Usuario) REFERENCES usuario(ID_Usuario),
    FOREIGN KEY (ID_Premio) REFERENCES premio(ID_Premio)
);

-- Insertar valores de ejemplo en la tabla usuario
INSERT INTO usuario (Nombre_Usuario, mail, Password, Victorias)
VALUES
    ('Sergi', 'maria@example.com', 'hash_password_123', 5),
    ('Yerald', 'sergio@example.com', 'hash_password_456', 10),
    ('Nizar', 'ana@example.com', 'hash_password_789', 3);

-- Insertar valores de ejemplo en la tabla partida
INSERT INTO partida (fecha, N_Jugadores, Jugador1, Jugador2, Jugador3, Jugador4)
VALUES
    ('2024-10-01', 4, 1, 2, 3, NULL),
    ('2024-10-02', 3, 2, 1, 3, NULL),
    ('2024-10-03', 2, 2, 3, NULL, NULL);

-- Insertar valores de ejemplo en la tabla premio
INSERT INTO premio (Nombre_Premio)
VALUES
    ('Medalla de Bronce'),
    ('Medalla de Plata'),
    ('Medalla de Oro'),
    ('Medalla Diamante');

-- Insertar ejemplos de premios obtenidos por usuarios en la tabla premio_usuario
INSERT INTO premio_usuario (ID_Usuario, ID_Premio, Fecha_Obtencion)
VALUES
    (1, 3, '2024-10-04'),
    (2, 4, '2024-10-05'),
    (3, 2, '2024-10-06');
