#  Microplantas

## Dependências

Instale a CLI do [platformio](http://platformio.org).

Usando `pip`:

		pip install -U platformio

[Instruções detalhadas](http://docs.platformio.org/en/stable/installation.html).

## Build

### Compilar/verificar

		platformio run

### Upload do programa

		platformio run -t upload

É possível definir qual será a porta utilizada para fazer upload e qual `environment` (placa) para compilação.
Ex.:

		platformio run -e uno -t upload --upload-port /dev/ttyACM0 

Ver platformio.ini para `environments` configurados. Mais informações na [documentação](http://docs.platformio.org/en/stable/projectconf.html).

### Observar a serial:

		platformio device monitor

Utilize o argumento `-p` (ou --port) para definir qual porta serial utilizar. Ex.:

		platformio device monitor -p /dev/ttyACM0
