# Protocolo

## Leitura de sensores
TO-DO ...

## Serviços (atuadores)

Cada nó no sistema, além de possuir sensores que podem ser acessados, pode oferecer "serviços" que permitem ativar o comportamento de um ou mais atuadores (físicos ou digitais).

O fluxo principal de ativação de um serviço é descrito a seguir:

1. Cliente (através da "bridge") envia comando (em JSON) para ativar serviço. Valor inclui nome do comando, # do nó alvo e pode incluir argumentos (opcionais).
Ex.:
```json
	{ "#" : 1, "cmd" : "regar", "duration" : 200}
```
2. Nó sensor verifica comando e inicia execução do serviço. Ele retorna uma mensagem codificada como um objeto JSON, contendo o resultado da requisição (seguindo codificação de status utilizados em HTTP). Pode haver também uma mensagem para explicar o resultado (erro) caso ocorra.

Ex.:
```json
	{ "res" : 200}
	{ "res" : 404, "msg" : "service does not exist"}
```
3. Nó sensor passa a enviar notificações para o cliente sobre o progresso do serviço. Cada notificação irá conter o id de um nó e o tipo do evento. Além disso irá conter 1 ou mais objetos, identificados pelo nome do serviço, contendo os atributos correspondendo ao estado atual do serviço.

Ex.:
```json
	{ "#" : 1, "evt": "notify", "regar" : {"prog" : 85.0}}
```

## Queries

Clientes podem fazer consultas em relação ao nós de um sensor.

#### 1. Listar nós:

entrada: 
```json
	{ "cmd": "nodes"}
```

saída: lista com ids de nós
```json
	[1, 2, 3, 5]
```

#### 2. Listar serviços:

entrada: 
```json
	{ "#" : 2, "cmd": "services"}
```

saída:
```json
	["regar", "iluminar"]
```

#### 3. Obter status de serviço:

entrada: 
```json
	{ "#" : 2, "cmd": "get", "serv" : "regar"}
```

saída:

* em execução: inclui status variável conforme serviço
```json
{ "#" : 2, "regar" : {"prog" : 85.0}}
```
* serviço parado: valor é objeto vazio
```json
{ "#" : 2, "regar" : {}}
```