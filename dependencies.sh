#bin-bash!
#Correrse en super usuario
echo "Actulizando y descargando dependencias..."
sudo dnf update
sudo dnf install gcc
sudo dnf install make
echo "Compilando Programa..."
make

if [ $? -eq 0 ]; then
    echo "La compilación fue exitosa."
    ./executable
else
    echo "Hubo un error durante la compilación."
    exit 1
fi


echo "Proceso completado."



