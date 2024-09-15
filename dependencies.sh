#bin-bash!
#Correrse en super usuario
echo "Actulizando y descargando dependencias..."
sudo dnf install gcc
sudo dnf install make
echo "Compilando Programa serial ..."
echo "Compresor serial ..."
make -f makefileSE

if [ $? -eq 0 ]; then
    echo "La compilación fue exitosa."
    ./encode
else
    echo "Hubo un error durante la compilación."
    exit 1
fi
echo "Descompresor serial ..."
make -f makefileSD

if [ $? -eq 0 ]; then
    echo "La compilación fue exitosa."
    ./decode
else
    echo "Hubo un error durante la compilación."
    exit 1
fi

echo "Compilando Programa Paralelo..."
echo "Compresor Fork ..."
make -f makefileFE

if [ $? -eq 0 ]; then
    echo "La compilación fue exitosa."
    ./fencode
else
    echo "Hubo un error durante la compilación."
    exit 1
fi
echo "Descompresor Fork ..."
make -f makefileFD

if [ $? -eq 0 ]; then
    echo "La compilación fue exitosa."
    ./fdecode
else
    echo "Hubo un error durante la compilación."
    exit 1
fi

echo "Compilando Programa concurrente..."
echo "Compresor threads ..."
make -f makefileTE

if [ $? -eq 0 ]; then
    echo "La compilación fue exitosa."
    ./tencode
else
    echo "Hubo un error durante la compilación."
    exit 1
fi
echo "Descompresor Fork ..."
make -f makefileTD

if [ $? -eq 0 ]; then
    echo "La compilación fue exitosa."
    ./tdecode
else
    echo "Hubo un error durante la compilación."
    exit 1
fi


echo "Proceso completado."



