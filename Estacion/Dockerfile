#python 2.7
FROM python:2.7

# directorio de trabajo
WORKDIR /Estacion

# Añadir directorio actual
ADD . /Estacion

# Instalar dependencias
RUN pip install --trusted-host pypi.python.org -r dep.txt

# Iniciar el programa

CMD ["bash","start.sh"]

