#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
using namespace std;


//Obtiene el numero de variables de decision, el numero de restricciones, el tipo de restricciones y los coeficientes de la matriz y la funcion objetiov

int obtieneDatosProblema( int tipoRestricciones[], int& numeroVariables, int& numeroRestricciones, float FO[] , float matriz[50][50], int &variablesExtras);


int definirMetodo( float matriz[50][50] , float FO[] , int tipoRestricciones[] , int& numeroRestricciones, int& numeroVariables, int& variablesExtras);


void simplexBasico( float matriz[50][50] , float FO[] , int tipoRestricciones[] , int& numeroRestricciones, int& numeroVariables, int& variablesExtras);


void simplexMIN( float matriz[50][50] , float FO[] , int tipoRestricciones[] , int& numeroRestricciones, int& numeroVariables, int& variablesExtras);

//Si hay un numero positivo en el lado derecho esta funcion corrige la matriz y multiplica por -1 todas las filas con un numero negatiov en el lado derecho
void positivoLD(float matriz[50][50] , int tipoRestricciones[] , int& numeroRestricciones, int& numeroVariables, int& variablesExtras);


//Esta funcion coloca las variables artificiales en la funcion objetivo
void poneArtificiales(float FO[] , int tipoRestricciones[] , int& numeroRestricciones, int& numeroVariables, int& variablesExtras, int operacion );


void simplexMAX( float matriz[50][50] , float FO[] , int tipoRestricciones[] , int& numeroRestricciones, int& numeroVariables, int& variablesExtras);

void Portada();
void Intro();

int main()
{
    float matriz[50][50];
    int numeroVariables, numeroRestricciones; 
    int objetivo;
    float FO[25];
    int tipoRestricciones[10];
    int variablesExtras=0;  //Corresponde al numero de variables de holgura o artificiales que se agreguen al problema
    int metodo=0; //Esta variable nos va a permitir definir si hacemos normal o dos fases, si es 0 es normal si es 1 es dos fases
    int numeroColumnas=0; //Aqui se le asginara el valor de numero de variables mas el numoer de variables extras

    Portada();
    Intro();
    
	objetivo= obtieneDatosProblema(tipoRestricciones, numeroVariables, numeroRestricciones, FO, matriz, variablesExtras);
     
    

    if( objetivo== 1 )
    {
        metodo= definirMetodo( matriz, FO, tipoRestricciones, numeroRestricciones, numeroVariables, variablesExtras); 
        if(metodo==0)
        {
            printf("\vamos a realizar un metodo simplex basico: ");
            simplexBasico(matriz, FO, tipoRestricciones, numeroRestricciones, numeroVariables, variablesExtras);
        }else
        {
            positivoLD(matriz, tipoRestricciones, numeroRestricciones, numeroVariables, variablesExtras);
            printf("\nEl metodo que vamos a realizar es el de la M: ");

            simplexMAX(matriz, FO ,tipoRestricciones, numeroRestricciones, numeroVariables, variablesExtras);

        }
    }
    else
    {
        printf("\nEl metodo que vamos a realizar es el de la M, porque buscamos minimizar: ");
        positivoLD(matriz, tipoRestricciones, numeroRestricciones, numeroVariables, variablesExtras);

        simplexMIN(matriz, FO ,tipoRestricciones, numeroRestricciones, numeroVariables, variablesExtras);
    }



    //printf("%d   %d   %d   %d " , numeroVariables, numeroRestricciones, objetivo , variablesExtras); Imprimer los varoles del numero de variavbles, res, si minimiza o maximiza y el numero de variables de holgura con las artificiales 

    numeroColumnas=variablesExtras + numeroVariables;

    getch();
    return 0;
}

void poneArtificiales(float FO[] , int tipoRestricciones[] , int& numeroRestricciones, int& numeroVariables, int& variablesExtras, int operacion )
{
    int i;
    int aux=0;
    int signo;

    if(operacion==0)
    {
        signo=1;
    }
    else if(operacion==1)
    {
        signo=-1;
    }

    for (i = 0; i < numeroRestricciones ; i++)
    {
        if(tipoRestricciones[i] ==1)
        {
            printf("\nAGREGANDO %d: " , numeroVariables+aux+2);
            FO[numeroVariables+aux + 1] = 1000000 *signo;
            aux=aux+2;
        }
        else if(tipoRestricciones[i]==2 )
        {
            printf("\nAGREGANDO %d: " , numeroVariables+aux+1);
            FO[numeroVariables +aux  ] =1000000 * signo;
            aux++;
        }else
        {
            FO[numeroVariables + aux ]=0;
            aux++;
        }
        
    }
    printf("\n\nAcontinuacion puedes deducir cuales columnas corresponden a las variables artificiales \n\n");
    for (i = 0; i < numeroVariables+variablesExtras+1; i++)
    {
        if( i ==numeroVariables+variablesExtras)
        {
            printf(" = Z" , i+1);
        }
        else
        {
            printf("+ (%f) x%d " , FO[i] , i+1);
        }
    }
    

}

void simplexMIN( float matriz[50][50] , float FO[] , int tipoRestricciones[] , int& numeroRestricciones, int& numeroVariables, int& variablesExtras)
{
    int i,j, k=0;
    int ponArtificial[20];
    int M=1000000 ;
    float vectorAux[30];
    float FO2[30];
    

    //variables intento copy page

    int aux=0; //Nos dice si vamos a realizar otra iteracion o ya no
    float resultados[30];
    int columnaPivote=0;
    int filaPivote=0;
    float mayorFO=   -10000000;
    float menorLD= 100000000; 
    float elementoPivote=0; // Esta variable nos guarda el coeficiente del elemento pivote 
    float vectorAuxiliar[25]; //Esta variable es la que me va permitir hacer ceros los elementos de arriba y abajp del pivote
    float coeficienteAuxiliar =0 ; //Sirve para hacer ceros arriba y abajo del elemento pivote, va a tomar el inverso aditivo 
    int resgistroEntrada[30], resgistroSalida[30]; //Ayuda a definir los resultados finales
    
    
    poneArtificiales(FO, tipoRestricciones, numeroRestricciones, numeroVariables, variablesExtras, 0);
    //GUARDANFO FO2 QUE ES FO PERO EN NEGATIVO
    for (i = 0; i < numeroVariables+variablesExtras +1; i++)
    {
        if (i==numeroVariables+variablesExtras)
        {
            FO2[i]= 0;
        }
        else
        {
        FO2[i] = - FO[i];
        }
    }

    //imprimiendo fo2 
    printf("\n\n Igualamos a cero la funcion objetivo\n\n");
    for (i = 0; i < numeroVariables+variablesExtras+1; i++)
    {
        if( i ==numeroVariables+variablesExtras)
        {
            printf(" = %f " , FO2[i]);
        }
        else
        {
            printf("+ (%f) x%d " , FO2[i] , i+1);
        }
    }

    //limpiamos el vector aux
    for (i = 0; i < 30; i++)
    {
        vectorAux[i]=0;
    }

    //hacemos el proceso de eliminar las variables artificiales dentro de la funcion objetivo
    for (i = 0; i < numeroRestricciones; i++)
    {
        if( tipoRestricciones [i] == 1 || tipoRestricciones[i] == 2)
        {
            for ( j = 0; j < numeroVariables +variablesExtras+1; j++)
            {
                vectorAux[j] += matriz[i][j] * M;
            }   
        }
    }
    
    //Asignamos los nuevos valores de la funcion objetivo
    for (i = 0; i < numeroVariables+variablesExtras+1; i++)
    {
        FO2[i] = FO2[i] + vectorAux[i];
    }
    
    printf("\n\nFuncion objetivo sin las variables artificiales que habiamos agregado\n\n");
    for (i = 0; i < numeroVariables+variablesExtras+1; i++)
    {
        if( i ==numeroVariables+variablesExtras)
        {
            printf(" = %d " , FO2[i]);
        }
        else
        {
            printf("+ (%f) x%d " , FO2[i] , i+1);
        }
    }



    //intento de copy page

    for (i = 0; i < 20; i++)
    {
        resultados[i]=0;
    }
    
    do
    {
        //Limpiamos las variables auxiliares
        aux=0;  
        elementoPivote=0;
        menorLD=1000000000;
        mayorFO=-10000000;

        for (i = 0; i < numeroVariables +variablesExtras+1; i++)
        {
            vectorAuxiliar[i] =0;
        }
        
        coeficienteAuxiliar=0;

        //Buscamos la columna pivote
        for(i=0; i < numeroVariables + variablesExtras  ; i++)
        {
            if( FO2[i] >0 ) //Esta condicion nos dice que si encontramos un positivo en la FO se puede seguir
            {
                aux=1; //Se encontro un positivo

                //printf("\nSe encontronun negativo puedes seguir\n\n");
            }
            
            if( FO2[i] > mayorFO )   //Quiza este if se haga en otro for similar
            {
                mayorFO= FO2[i];
                columnaPivote= i;
                //printf("\nposible mayor en %d" , i+1);
            }
            
        }


        //Buscamos la fila pivote
        if(aux==1)
        {
            printf("\nITERACION %d:  "   , k);
            for (i = 0; i < numeroRestricciones  ; i++)
            {
                if( matriz[i][columnaPivote] !=0 && matriz[i][columnaPivote] > 0  )
                {
                    if( matriz[i][numeroVariables + variablesExtras] / matriz[i][columnaPivote] <menorLD )
                    {
                        filaPivote= i;
                        menorLD = matriz[i][numeroVariables + variablesExtras] / matriz[i][columnaPivote] ; // No dividir entre cero
                        //printf("\n %f valor de dividir  entonces es el nuevomenor \n  %f  menor" ,  matriz[i][numeroVariables + variablesExtras] / matriz[i][columnaPivote] , menorLD);
                    }
                }   
            }   

            elementoPivote=matriz[filaPivote][columnaPivote];

            printf("\n\n");
            printf("\n\nDIVIDIENDO LA FILA %d (FILA PIVOTE) POR EL ELEMENTO PIVOETE %f en columna %d " , filaPivote+1, matriz[filaPivote][columnaPivote], columnaPivote+1 );
            for (i = 0; i < numeroVariables + variablesExtras+1; i++)
            {
                matriz[filaPivote][i] = matriz[filaPivote][i] / elementoPivote;
            }
            //imprimiendo de prueba
            
            for ( i = 0; i < numeroRestricciones ; i++)
            {   
                printf("\n");
                for ( j = 0; j <numeroVariables+ variablesExtras+1 ; j++)
                {
                    if(j==numeroVariables +variablesExtras)
                    {
                        printf(" =  %f" , matriz[i][j]  );
                    }
                    else{
                    printf(" + (%f) x%d"  , matriz[i][j] ,j+1);
                    }
                }
            }

            //Haciendo ceros los numero arriba del pivote
            for (i = 0; i < filaPivote ; i++)
            {
                coeficienteAuxiliar= matriz[i][columnaPivote];

                for ( j = 0; j < numeroVariables + variablesExtras+1; j++)
                {
                    vectorAuxiliar[j] = matriz[filaPivote][j] * coeficienteAuxiliar;
                    
                }

                for ( j = 0; j < numeroVariables+variablesExtras +1; j++)
                {
                    matriz[i][j] =  matriz[i] [j] - vectorAuxiliar[j];
                }
            }

            //Haciendo ceros los numeros abajo del pivote
            for (i = filaPivote +1 ; i < numeroRestricciones ; i++)
            {
                coeficienteAuxiliar= matriz[i][columnaPivote];

                for ( j = 0; j < numeroVariables + variablesExtras+1; j++)
                {
                    vectorAuxiliar[j] = matriz[filaPivote][j] * coeficienteAuxiliar;
                    
                }

                for ( j = 0; j < numeroVariables+variablesExtras +1; j++)
                {
                    matriz[i][j] =  matriz[i] [j] - vectorAuxiliar[j];
                }
                
            }

            //Haciendo cero el elemnto de FO
            for (i = 0; i < numeroVariables+variablesExtras+1; i++)
            {
                coeficienteAuxiliar= FO2[columnaPivote];

                for ( j = 0; j < numeroVariables + variablesExtras+1; j++)
                {
                    vectorAuxiliar[j] = matriz[filaPivote][j] * coeficienteAuxiliar;
                }

                for ( j = 0; j < numeroVariables+variablesExtras +1; j++)
                {
                    FO2[j] =  FO2[j] - vectorAuxiliar[j];
                }
            }

            //imprimiendo de prueba
            printf("\n\nIMPRIMIENDO MATRIZ YA CON CEROS ARRIBA Y ABAJO DEL PIVOTE"); 

            printf("\nFO: \n");

            printf("\n");
            
            for (i = 0; i < numeroVariables + variablesExtras + 1; i++)
            {
                if( i== numeroVariables +variablesExtras )
                {
                    printf("= %f" ,  *(FO2 +i ));

                }else{
                    printf("  +  (%f x%d )  " , *(FO2 +i ), i+1);
                }
            }

            for ( i = 0; i < numeroRestricciones ; i++)
            {   
                printf("\n");
                for ( j = 0; j <numeroVariables+ variablesExtras+1 ; j++)
                {
                    if(j==numeroVariables +variablesExtras)
                    {
                        printf(" =  %f" , matriz[i][j]  );
                    }
                    else{
                    printf(" + (%f) x%d"  , matriz[i][j] ,j+1);
                    }
                }
            }
            printf("\n\nTermino la iteracion %d" , k); 
            printf("\nEntro x%d " , columnaPivote+1);
            printf("\nSale la variable de la fila: %d" , filaPivote+1);
        }
        
        resgistroEntrada[k] = columnaPivote;
        resgistroSalida[k] = filaPivote;
        
        if(aux==0)
        {
            printf("\nFin del metodo");
        }
        
        k++;
        printf("\n\n");
    }while(aux==1);

    int guarda;
    for ( i = 0; i < k-1 ; i++)
    {
        guarda=1;
        for ( j = i+1; j < k-1 ; j++)
        {
            if(guarda!=0)
            {
                if( resgistroSalida[i] != resgistroSalida[j] )
                {
                    guarda=1;
                }
                else{
                    guarda=0;
                    
                }
            }   
        }
        
        if (guarda==1)
        {
            resultados[ resgistroEntrada[i] ] = matriz[ resgistroSalida[i] ][numeroVariables+variablesExtras];
            printf(" Guardando x%d en fila %d " , resgistroEntrada[i] , resgistroSalida[i] );
        }
        
        
    }

    if( FO2[numeroVariables+variablesExtras]  > M/10  )
    {
        printf("\nTu problema es infactible");

    }else
    {
        for (i = 0; i < k -1 ; i++)
        {
            printf("\n columna entra: %d \t fila sale: %d" , resgistroEntrada[i] +1 , resgistroSalida[i] +1 );
        } 

        for ( i = 0; i < numeroVariables ; i++)
        { 
            printf("\n x %d  =  %f", i +1, resultados[i]);   
        }

        printf("\nZ= %f" , FO2[numeroVariables+variablesExtras]);
        
    }


    system("pause");
}

void positivoLD(float matriz[50][50] , int tipoRestricciones[] , int& numeroRestricciones, int& numeroVariables, int& variablesExtras)
{
    int i, j;
    float matrizAux[50][50];
    int variablesExtras2;
    int bandera=0;
    int aux;

    for (i = 0; i < numeroRestricciones; i++)
    {
        if( matriz[i][numeroVariables + variablesExtras] < 0 )
        {
            printf("\nLa fila %d se va a multiplicar por -1 :)  " , i+1);
            for ( j = 0; j < numeroVariables + variablesExtras +1 ; j++)
            {
                matriz[i][ j] = matriz[ i] [j] * (-1);
            }

            if(tipoRestricciones[i] == 1 )
            {
                tipoRestricciones[i] =0;
            }
            else if(tipoRestricciones[i] == 0)
            {
                tipoRestricciones[i] =1;
            }
            printf("\ntipo restriccion %d : %d" , i+1 , tipoRestricciones[i] );
            bandera=1;
        }
    }

    if(bandera==1)
    {
        variablesExtras2=0;

        for ( i = 0; i < 50; i++)
        {
            for ( j = 0; j < 50; j++)
            {
                matrizAux[i][j]=0;
            }
            
        }
        
        //Contando el nuevo numero de variables extras
        for ( i = 0; i < numeroRestricciones ; i++)
        {
            if (tipoRestricciones[i] == 0)
            {
                variablesExtras2++;
            }
            else if(tipoRestricciones[i] ==1 )
            {
                variablesExtras2= variablesExtras2+2;
            }else if( tipoRestricciones[i] ==2)
            {
                variablesExtras2++;
            }
        }
        

        //Copiando solo los coeficientes de la variable de decision
        for (i = 0; i < numeroRestricciones; i++)
        {
            for (j = 0; j < numeroVariables ; j++)
            {
                matrizAux[i][j] = matriz[i][j];
            }
        }

        //Agregando las variables nuevas
        aux=0;
        for ( i = 0; i < numeroRestricciones ; i++)
        {
            if(tipoRestricciones[i] == 0)
            {
                matrizAux[i][numeroVariables +aux] = 1;
                aux++;
            }
            else if(tipoRestricciones[i] ==1 )
            {
                matrizAux[i][numeroVariables+aux] = -1;
                aux++;
                matrizAux[i][numeroVariables+aux] =1; 
                aux++;
            }
            else
            {
                matrizAux[i][numeroVariables+aux] = 1;
                aux++;
            }
            matrizAux[i][numeroVariables+variablesExtras2] =matriz[ i] [numeroVariables+variablesExtras];
        }

        printf("\n\t MATRIZ NUEVA");
        for ( i = 0; i < numeroRestricciones ; i++)
        {   
            printf("\n");
            for ( j = 0; j <numeroVariables+ variablesExtras2+1 ; j++)
            {
                if(j==numeroVariables +variablesExtras2)
                {
                    printf(" =  %f" , matrizAux[i][j]  );
                }
                else{
                printf(" + (%f) x%d"  , matrizAux[i][j] ,j+1);
                }
            }
        }
        printf("\n\t MATRIZ VIEJA ");
        for ( i = 0; i < numeroRestricciones ; i++)
        {   
            printf("\n");
            for ( j = 0; j <numeroVariables+ variablesExtras+1 ; j++)
            {
                if(j==numeroVariables +variablesExtras)
                {
                    printf(" =  %f" , matriz[i][j]  );
                }
                else{
                printf(" + (%f) x%d"  , matriz[i][j] ,j+1);
                }
            }
        }
        
        for ( i = 0; i < 50; i++) //REINICIAMOS MATRIZ
        {
            for ( j = 0; j < 50; j++)
            {
                matriz[i][j]=0;
            }
        }

        variablesExtras=variablesExtras2;
        for ( i = 0; i < numeroRestricciones ; i++)
        {
            for ( j = 0; j< numeroVariables +variablesExtras +1 ; j++)
            {
                matriz[i][j] = matrizAux[i][j];
                
            }
            
        } 
        printf("Nuevas variables extras: %d" , variablesExtras);
    }
    printf("\n\n");
}

void simplexMAX( float matriz[50][50] , float FO[] , int tipoRestricciones[] , int& numeroRestricciones, int& numeroVariables, int& variablesExtras)
{
    int i,j, k=0;
    int ponArtificial[20];
    int M=1000000 ;
    float vectorAux[30];
    float FO2[30];
    

    //variables intento copy page

    int aux=0; //Nos dice si vamos a realizar otra iteracion o ya no
    float resultados[30];
    int columnaPivote=0;
    int filaPivote=0;
    float menorFO=   10000000;
    float menorLD= 100000000; 
    float elementoPivote=0; // Esta variable nos guarda el coeficiente del elemento pivote 
    float vectorAuxiliar[25]; //Esta variable es la que me va permitir hacer ceros los elementos de arriba y abajp del pivote
    float coeficienteAuxiliar =0 ; //Sirve para hacer ceros arriba y abajo del elemento pivote, va a tomar el inverso aditivo 
    int resgistroEntrada[30], resgistroSalida[30]; //Ayuda a definir los resultados finales
    
    
    poneArtificiales(FO, tipoRestricciones, numeroRestricciones, numeroVariables, variablesExtras, 1);
    //GUARDANFO FO2 QUE ES FO PERO EN NEGATIVO
    for (i = 0; i < numeroVariables+variablesExtras +1; i++)
    {
        if (i==numeroVariables+variablesExtras)
        {
            FO2[i]= 0;
        }
        else
        {
        FO2[i] = - FO[i];
        }
    }

    //imprimiendo fo2 
    printf("\n\n Igualamos a cero la funcion objetivo\n\n");
    for (i = 0; i < numeroVariables+variablesExtras+1; i++)
    {
        if( i ==numeroVariables+variablesExtras)
        {
            printf(" = %f " , FO2[i]);
        }
        else
        {
            printf("+ (%f) x%d " , FO2[i] , i+1);
        }
    }

    //limpiamos el vector aux
    for (i = 0; i < 30; i++)
    {
        vectorAux[i]=0;
    }

    //hacemos el proceso de eliminar las variables artificiales dentro de la funcion objetivo
    for (i = 0; i < numeroRestricciones; i++)
    {
        if( tipoRestricciones [i] == 1 || tipoRestricciones[i] == 2)
        {
            for ( j = 0; j < numeroVariables +variablesExtras+1; j++)
            {
                vectorAux[j] += matriz[i][j] *  (-M);
            }   
        }
    }
    
    //Asignamos los nuevos valores de la funcion objetivo
    for (i = 0; i < numeroVariables+variablesExtras+1; i++)
    {
        FO2[i] = FO2[i] + vectorAux[i];
    }
    
    printf("\n\nFuncion objetivo sin las variables artificiales que habiamos agregado\n\n");
    for (i = 0; i < numeroVariables+variablesExtras+1; i++)
    {
        if( i ==numeroVariables+variablesExtras)
        {
            printf(" = %d " , FO2[i]);
        }
        else
        {
            printf("+ (%f) x%d " , FO2[i] , i+1);
        }
    }



    //intento de copy page

    for (i = 0; i < 20; i++)
    {
        resultados[i]=0;
    }
    
    do
    {
        //Limpiamos las variables auxiliares
        aux=0;  
        elementoPivote=0;
        menorLD=1000000000;
        menorFO=10000000;

        for (i = 0; i < numeroVariables +variablesExtras+1; i++)
        {
            vectorAuxiliar[i] =0;
        }
        
        coeficienteAuxiliar=0;

        //Buscamos la columna pivote
        for(i=0; i < numeroVariables + variablesExtras  ; i++)
        {
            if( FO2[i] < 0 ) //Esta condicion nos dice que si encontramos un NEGAtivo en la FO se puede seguir
            {
                aux=1; //Se encontro un positivo

                //printf("\nSe encontronun negativo puedes seguir\n\n");
            }
            
            if( FO2[i] < menorFO )   //Quiza este if se haga en otro for similar
            {
                menorFO= FO2[i];
                columnaPivote= i;
                //printf("\nposible mayor en %d" , i+1);
            }
            
        }


        //Buscamos la fila pivote
        if(aux==1)
        {
            printf("\nITERACION %d:  "   , k);
            for (i = 0; i < numeroRestricciones  ; i++)
            {
                if( matriz[i][columnaPivote] !=0 && matriz[i][columnaPivote] > 0  )
                {
                    if( matriz[i][numeroVariables + variablesExtras] / matriz[i][columnaPivote] <menorLD )
                    {
                        filaPivote= i;
                        menorLD = matriz[i][numeroVariables + variablesExtras] / matriz[i][columnaPivote] ; // No dividir entre cero
                        //printf("\n %f valor de dividir  entonces es el nuevomenor \n  %f  menor" ,  matriz[i][numeroVariables + variablesExtras] / matriz[i][columnaPivote] , menorLD);
                    }
                }   
            }   

            elementoPivote=matriz[filaPivote][columnaPivote];

            printf("\n\n");
            printf("\n\nDIVIDIENDO LA FILA %d (FILA PIVOTE) POR EL ELEMENTO PIVOETE %f en columna %d " , filaPivote+1, matriz[filaPivote][columnaPivote], columnaPivote+1 );
            for (i = 0; i < numeroVariables + variablesExtras+1; i++)
            {
                matriz[filaPivote][i] = matriz[filaPivote][i] / elementoPivote;
            }
            //imprimiendo de prueba
            
            for ( i = 0; i < numeroRestricciones ; i++)
            {   
                printf("\n");
                for ( j = 0; j <numeroVariables+ variablesExtras+1 ; j++)
                {
                    if(j==numeroVariables +variablesExtras)
                    {
                        printf(" =  %f" , matriz[i][j]  );
                    }
                    else{
                    printf(" + (%f) x%d"  , matriz[i][j] ,j+1);
                    }
                }
            }

            //Haciendo ceros los numero arriba del pivote
            for (i = 0; i < filaPivote ; i++)
            {
                coeficienteAuxiliar= matriz[i][columnaPivote];

                for ( j = 0; j < numeroVariables + variablesExtras+1; j++)
                {
                    vectorAuxiliar[j] = matriz[filaPivote][j] * coeficienteAuxiliar;
                    
                }

                for ( j = 0; j < numeroVariables+variablesExtras +1; j++)
                {
                    matriz[i][j] =  matriz[i] [j] - vectorAuxiliar[j];
                }
            }

            //Haciendo ceros los numeros abajo del pivote
            for (i = filaPivote +1 ; i < numeroRestricciones ; i++)
            {
                coeficienteAuxiliar= matriz[i][columnaPivote];

                for ( j = 0; j < numeroVariables + variablesExtras+1; j++)
                {
                    vectorAuxiliar[j] = matriz[filaPivote][j] * coeficienteAuxiliar;
                    
                }

                for ( j = 0; j < numeroVariables+variablesExtras +1; j++)
                {
                    matriz[i][j] =  matriz[i] [j] - vectorAuxiliar[j];
                }
                
            }

            //Haciendo cero el elemnto de FO
            for (i = 0; i < numeroVariables+variablesExtras+1; i++)
            {
                coeficienteAuxiliar= FO2[columnaPivote];

                for ( j = 0; j < numeroVariables + variablesExtras+1; j++)
                {
                    vectorAuxiliar[j] = matriz[filaPivote][j] * coeficienteAuxiliar;
                }

                for ( j = 0; j < numeroVariables+variablesExtras +1; j++)
                {
                    FO2[j] =  FO2[j] - vectorAuxiliar[j];
                }
            }

            //imprimiendo de prueba
            printf("\n\nIMPRIMIENDO MATRIZ YA CON CEROS ARRIBA Y ABAJO DEL PIVOTE"); 

            printf("\nFO: \n");

            printf("\n");
            
            for (i = 0; i < numeroVariables + variablesExtras + 1; i++)
            {
                if( i== numeroVariables +variablesExtras )
                {
                    printf("= %f" ,  *(FO2 +i ));

                }else{
                    printf("  +  (%f x%d )  " , *(FO2 +i ), i+1);
                }
            }

            for ( i = 0; i < numeroRestricciones ; i++)
            {   
                printf("\n");
                for ( j = 0; j <numeroVariables+ variablesExtras+1 ; j++)
                {
                    if(j==numeroVariables +variablesExtras)
                    {
                        printf(" =  %f" , matriz[i][j]  );
                    }
                    else{
                    printf(" + (%f) x%d"  , matriz[i][j] ,j+1);
                    }
                }
            }
            printf("\n\nTermino la iteracion %d" , k); 
            printf("\nEntro x%d " , columnaPivote+1);
            printf("\nSale la variable de la fila: %d" , filaPivote+1);
        }
        
        resgistroEntrada[k] = columnaPivote;
        resgistroSalida[k] = filaPivote;
        
        if(aux==0)
        {
            printf("\nFin del metodo");
        }
        
        k++;
        printf("\n\n");
    }while(aux==1);

    int guarda;
    for ( i = 0; i < k-1 ; i++)
    {
        guarda=1;
        for ( j = i+1; j < k-1 ; j++)
        {
            if(guarda!=0)
            {
                if( resgistroSalida[i] != resgistroSalida[j] )
                {
                    guarda=1;
                }
                else{
                    guarda=0;
                    
                }
            }   
        }
        
        if (guarda==1)
        {
            resultados[ resgistroEntrada[i] ] = matriz[ resgistroSalida[i] ][numeroVariables+variablesExtras];
            printf(" Guardando x%d en fila %d " , resgistroEntrada[i] , resgistroSalida[i] );
        }
        
        
    }

    if( FO2[numeroVariables+variablesExtras]  > M/10  )
    {
        printf("\nTu problema es infactible");

    }else
    {
        for (i = 0; i < k -1 ; i++)
        {
            printf("\n columna entra: %d \t fila sale: %d" , resgistroEntrada[i] +1 , resgistroSalida[i] +1 );
        } 

        for ( i = 0; i < numeroVariables ; i++)
        { 
            printf("\n x %d  =  %f", i +1, resultados[i]);   
        }

        printf("\nZ= %f" , FO2[numeroVariables+variablesExtras]);
        
    }


    system("pause");
}


void simplexBasico( float matriz[50][50] , float FO[] , int tipoRestricciones[] , int& numeroRestricciones, int& numeroVariables, int& variablesExtras)
{

    int i,j=0, k =0;
    int aux=0; //Nos dice si vamos a realizar otra iteracion o ya no
    float resultados[30];
    int columnaPivote=0;
    int filaPivote=0;
    float menorFO=  10000000;
    float menorLD= 100000000; 
    float FO2[25];  //Es la misma funcion objetivo pero con el signo negativo
    float elementoPivote=0; // Esta variable nos guarda el coeficiente del elemento pivote 
    float vectorAuxiliar[25]; //Esta variable es la que me va permitir hacer ceros los elementos de arriba y abajp del pivote
    float coeficienteAuxiliar =0 ; //Sirve para hacer ceros arriba y abajo del elemento pivote, va a tomar el inverso aditivo 
    int resgistroEntrada[30], resgistroSalida[30]; //Ayuda a definir los resultados finales

    for (i = 0; i < numeroVariables + variablesExtras +1 ; i++) //FO2 NO ES MAS QUE F0 PERO MULTIPLICADA POR UN MENOS 1
    {
        FO2[i] = -FO[i];
    }
    
    for ( i = 0; i < numeroVariables; i++) //limpiando el vector resultado
    {
       
        resultados[i]=0 ;
        
    }    

    do
    {
        //Limpiamos las variables auxiliares
        aux=0;  
        elementoPivote=0;
        menorLD=1000000000;
        menorFO=10000000;

        for (i = 0; i < numeroVariables +variablesExtras+1; i++)
        {
            vectorAuxiliar[i] =0;
        }
        
        coeficienteAuxiliar=0;

        //Buscamos la columna pivote
        for(i=0; i < numeroVariables + variablesExtras  ; i++)
        {
            if( FO2[i] < 0 ) //Esta condicion nos dice que si encontramos un negativo en la FO se puede seguir
            {
                aux=1; //Se encontro un negativo 

                //printf("\nSe encontronun negativo puedes seguir\n\n");
            }
            
            if( FO2[i] < menorFO )   //Quiza este if se haga en otro for similar
            {
                menorFO= FO2[i];
                columnaPivote= i;
            }
            
        }


        //Buscamos la fila pivote
        if(aux==1)
        {
            printf("\nITERACION %d:  "   , k);
            for (i = 0; i < numeroRestricciones  ; i++)
            {
                if( matriz[i][columnaPivote] !=0 && matriz[i][columnaPivote] > 0  )
                {
                    if( matriz[i][numeroVariables + variablesExtras] / matriz[i][columnaPivote] <menorLD )
                    {
                        filaPivote= i;
                        menorLD = matriz[i][numeroVariables + variablesExtras] / matriz[i][columnaPivote] ; // No dividir entre cero
                        //printf("\n %f valor de dividir  entonces es el nuevomenor \n  %f  menor" ,  matriz[i][numeroVariables + variablesExtras] / matriz[i][columnaPivote] , menorLD);
                    }
                }   
            }   

            elementoPivote=matriz[filaPivote][columnaPivote];

            printf("\n\n");
            printf("\n\nDIVIDIENDO LA FILA %d (FILA PIVOTE) POR EL ELEMENTO PIVOETE %f en columna %d " , filaPivote+1, matriz[filaPivote][columnaPivote], columnaPivote+1 );
            for (i = 0; i < numeroVariables + variablesExtras+1; i++)
            {
                matriz[filaPivote][i] = matriz[filaPivote][i] / elementoPivote;
            }
            //imprimiendo de prueba
            
            for ( i = 0; i < numeroRestricciones ; i++)
            {   
                printf("\n");
                for ( j = 0; j <numeroVariables+ variablesExtras+1 ; j++)
                {
                    if(j==numeroVariables +variablesExtras)
                    {
                        printf(" =  %f" , matriz[i][j]  );
                    }
                    else{
                    printf(" + (%f) x%d"  , matriz[i][j] ,j+1);
                    }
                }
            }

            //Haciendo ceros los numero arriba del pivote
            for (i = 0; i < filaPivote ; i++)
            {
                coeficienteAuxiliar= matriz[i][columnaPivote];

                for ( j = 0; j < numeroVariables + variablesExtras+1; j++)
                {
                    vectorAuxiliar[j] = matriz[filaPivote][j] * coeficienteAuxiliar;
                    
                }

                for ( j = 0; j < numeroVariables+variablesExtras +1; j++)
                {
                    matriz[i][j] =  matriz[i] [j] - vectorAuxiliar[j];
                }
            }

            //Haciendo ceros los numeros abajo del pivote
            for (i = filaPivote +1 ; i < numeroRestricciones ; i++)
            {
                coeficienteAuxiliar= matriz[i][columnaPivote];

                for ( j = 0; j < numeroVariables + variablesExtras+1; j++)
                {
                    vectorAuxiliar[j] = matriz[filaPivote][j] * coeficienteAuxiliar;
                    
                }

                for ( j = 0; j < numeroVariables+variablesExtras +1; j++)
                {
                    matriz[i][j] =  matriz[i] [j] - vectorAuxiliar[j];
                }
                
            }

            //Haciendo cero el elemnto de FO
            for (i = 0; i < numeroVariables+variablesExtras+1; i++)
            {
                coeficienteAuxiliar= FO2[columnaPivote];

                for ( j = 0; j < numeroVariables + variablesExtras+1; j++)
                {
                    vectorAuxiliar[j] = matriz[filaPivote][j] * coeficienteAuxiliar;
                }

                for ( j = 0; j < numeroVariables+variablesExtras +1; j++)
                {
                    FO2[j] =  FO2[j] - vectorAuxiliar[j];
                }
            }

            //imprimiendo de prueba
            printf("\n\nIMPRIMIENDO MATRIZ YA CON CEROS ARRIBA Y ABAJO DEL PIVOTE"); 

            printf("\nFO: \n");

            printf("\n");
            
            for (i = 0; i < numeroVariables + variablesExtras + 1; i++)
            {
                if( i== numeroVariables +variablesExtras )
                {
                    printf("= %f" ,  *(FO2 +i ));

                }else{
                    printf("  +  (%f x%d )  " , *(FO2 +i ), i+1);
                }
            }

            for ( i = 0; i < numeroRestricciones ; i++)
            {   
                printf("\n");
                for ( j = 0; j <numeroVariables+ variablesExtras+1 ; j++)
                {
                    if(j==numeroVariables +variablesExtras)
                    {
                        printf(" =  %f" , matriz[i][j]  );
                    }
                    else{
                    printf(" + (%f) x%d"  , matriz[i][j] ,j+1);
                    }
                }
            }
            printf("\n\nTermino la iteracion %d" , k); 
            printf("\nEntro x%d " , columnaPivote+1);
            printf("\nSale la variable de la fila: %d" , filaPivote+1);
        }
        
        resgistroEntrada[k] = columnaPivote;
        resgistroSalida[k] = filaPivote;
        
        if(aux==0)
        {
            printf("\nFin del metodo");
        }
        
        k++;
        printf("\n\n");
    }while(aux==1);

    int guarda;
    for ( i = 0; i < k-1 ; i++)
    {
        guarda=1;
        for ( j = i+1; j < k-1 ; j++)
        {
            if(guarda!=0)
            {
                if( resgistroSalida[i] != resgistroSalida[j] )
                {
                    guarda=1;
                }
                else{
                    guarda=0;
                    
                }
            }   
        }
        
        if (guarda==1)
        {
            resultados[ resgistroEntrada[i] ] = matriz[ resgistroSalida[i] ][numeroVariables+variablesExtras];
            printf(" Guardando x%d en fila %d " , resgistroEntrada[i] , resgistroSalida[i] );
        }
        
        
    }

    for (i = 0; i < k -1 ; i++)
    {
        printf("\n columna entra: %d \t fila sale: %d" , resgistroEntrada[i] +1 , resgistroSalida[i] +1 );
    } 

    for ( i = 0; i < numeroVariables ; i++)
    { 
        printf("\n x %d  =  %f", i +1, resultados[i]);   
    }

    printf("\nZ= %f" , FO2[numeroVariables+variablesExtras]);
}


int definirMetodo( float matriz[50][50] , float FO[] , int tipoRestricciones[] , int& numeroRestricciones, int& numeroVariables, int& variablesExtras)
{
    int metodo=0, i, j;    


    for (i = 0; i < numeroRestricciones ; i++)
    {
        if(tipoRestricciones[i] != 0)
        {
            metodo=1;
            printf("\nHay una restriccion diferente a <= ");
            break;
        }
    }
    
    if(metodo==0)   //REVISAR CREO QUE NO ES NECESARIA ESTA REVISION
    {
        for(i=0; i < numeroVariables + variablesExtras; i++ )
        {
            if( FO[i] < 0 )
            {
                metodo=1;
                printf("\nTu FOO tiene coeficiente negativo");
                break;
            }
        }
    }

    if(metodo==0)
    {
        for( j=0; j< numeroRestricciones ; j++)
        {
            if( matriz[j][numeroVariables + variablesExtras] <0 )
            {
                metodo=1;
                printf("\nHay negativos en tu Lado derecho");
            }
        }
    }

    return metodo;
}

int obtieneDatosProblema( int tipoRestricciones[], int& numeroVariables, int& numeroRestricciones, float FO[], float matriz[50][50], int &variablesExtras)
{
    int objetivo; 
    int i, j;
    int confirmar;

    int aux=0; //Esta variable nos ayudara a colocar las variables de holgura y artificial en su lugar

    //Pidiendo el numero de variables, redtricciones y si se va a minimizar o maximizar 

    do{
        do
        {
        printf("\nDigite '0' si vas a minimizar o '1' si vas a maximizar: ");
        scanf("%d", &objetivo);
        }while( !(objetivo==0 || objetivo==1 ) );

        do
        {
        printf("\nDigite el numero de variables: ");
        scanf("%d", &numeroVariables);
        }while( numeroVariables >10 || numeroVariables<=1);

        do
        {
        printf("\nDigite el numero de restricciones: ");
        scanf("%d", &numeroRestricciones);
        }while( numeroRestricciones >10 || numeroRestricciones<=1 );


        printf("\nTu problema busca");
        if(objetivo== 0)
        {
            printf(" Minimizar ");
        }else
        {
            printf(" Maximizar ");
        }
        printf("\nAdemas, tu problema tiene %d variables de decision con %d restricciones" , numeroVariables, numeroRestricciones);
        printf("\n\n\nSi todos los datos son correctos por favor digita 1 para continuar, en caso contrario digite 0 y reingrese los datos: ");
        scanf("%d" , &confirmar);
    }while(confirmar!=1);
    system("puase");
    system("cls");



    //Pidiendo el tipo de restricciones
    printf("\nConocer el tipo de tus restricciones es importante ya que podemos saber cuantas variables de hoLgura o artificiales se van a ocupar");

    printf("\nDigitara '0' si tiene una restriccion del tipo <= ");
    printf("\nDigitara '1' si tiene una restriccion del tipo >= ");
    printf("\nDigitara '2' si tiene una restriccion del tipo = ");

    do{
        variablesExtras=0;
        for (i = 0; i < numeroRestricciones; i++)
        {   
            do
            {    
                printf("\nDigita el tipo de resctriccion %d: " , i+1);
                scanf("%d" , tipoRestricciones+i );
            }while( *(tipoRestricciones+i) != 0 &&  *(tipoRestricciones+i) != 1 && *(tipoRestricciones+i) != 2  );
        }


        printf("\nImprimiendo restricciones");
        for (i = 0; i < numeroRestricciones; i++)
        {
            if(*(tipoRestricciones+i) == 0 )
            {
                printf("\nRestriccion %d:   <= " , i+1,  *(tipoRestricciones+i) );
                variablesExtras++;
            }
            else if(*(tipoRestricciones+i) == 1 )
            {
                printf("\nRestriccion %d:   >= " , i+1,  *(tipoRestricciones+i) );
                variablesExtras=variablesExtras+2;
            }else if(*(tipoRestricciones+i) == 2 )
            {
                printf("\nRestriccion %d:   =" , i+1,  *(tipoRestricciones+i) );
                variablesExtras++;
            }
        }
        printf("\n\n\nSi todos los datos son correctos por favor digita 1 para continuar, en caso contrario digite 0 y reingrese los datos: ");
        scanf("%d" , &confirmar);
    }while(confirmar!=1);
    system("puase");
    system("cls");

    do{
        //Agrega coeficientes de la funcion objetivo
        printf("\n Agrega los coeficientes de tu funcion objetivo \n\n "); 

        for (i = 0; i < numeroVariables + variablesExtras + 1 ; i++)
        {
            if(i<numeroVariables)
            {
                printf("\nCoeficiente %d: " , i+1 );
                scanf("%f" , FO +i);
            }
            else
            {
                FO[i]= 0;
            }
        }
        printf("\n");
        //Vamos a llenar la matriz A con los coeficientes correspondientes

        for (i = 0; i < numeroRestricciones ; i++)  
        {
            printf("\n Vas a ingresar los coeficientes de la restriccion %d " , i+1);

            for (j = 0; j < numeroVariables + variablesExtras + 1 ; j++) //Se le suma uno porqye corresponde al coeficiente del lado derecho del a incecuacion
            {
                if(j==numeroVariables + variablesExtras)
                {
                    printf("\nDigite coeficiente del Lado derecho de la restriccion: "  ); 
                    scanf("%f" , &matriz[i][j] );
                }
                else if( j <numeroVariables)
                {
                    printf("\nDigite coeficiente de x%d: "  , j+1); 
                    scanf("%f" , &matriz[i][j] );
                }else
                {
                    matriz[i][j]=0;
                }
            }
            
        }







        aux=0; //Por si al momento de ingresar los datos se equiocan y quieren volver a reingresar los datos se aocmoden bien las variables extras
        
        //Este for se encarga de acomodar las variables de holgura y artificiales en su lugar
        for ( i = 0; i < numeroRestricciones ; i++)
        {
            if(tipoRestricciones[i] == 0)
            {
                matriz[i][numeroVariables +aux] = 1;
                aux++;
            }
            else if(tipoRestricciones[i] ==1 )
            {
                matriz[i][numeroVariables+aux] = -1;
                aux++;
                matriz[i][numeroVariables+aux] =1; 
                aux++;
            }
            else
            {
                matriz[i][numeroVariables+aux] = 1;
                aux++;
            }
            
        }

        system("puase");
        system("cls");

        //Vamos a imprimir la funcion objetivop y las restricciones para que el usuario vea si guardo bien su problema

        printf("\nFO: \n");

        printf("\n");
        
        for (i = 0; i < numeroVariables + variablesExtras + 1; i++)
        {
            if( i== numeroVariables +variablesExtras )
            {
                printf("= Z ");

            }else{
                printf("  +  (%f x%d )  " , *(FO +i ), i+1);
            }
        }

        printf("\nSujeto a: \n");

        for ( i = 0; i < numeroRestricciones ; i++)
        {   
            printf("\n");
            for ( j = 0; j <numeroVariables+ variablesExtras+1 ; j++)
            {
                if(j==numeroVariables +variablesExtras)
                {
                    printf(" =  %f" , matriz[i][j]  );
                }
                else{
                printf(" + (%f) x%d"  , matriz[i][j] ,j+1);
                }
            }
        }
        printf("\n\n\nSi todos los datos son correctos por favor digita 1 para continuar, en caso contrario digite 0 y reingrese los datos: ");
        scanf("%d" , &confirmar);
    }while(confirmar != 1);

    system("puase");
    system("cls");
    return objetivo;
}

void Portada()
{	
	printf("\n\n\t%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",201,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,187);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c                                   P R O Y E C T O  F I N A L                                 %c\n",186,186);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",204,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,185);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c                   U N I V E R S I D A D  A U T %c N O M A  D E  M %c X I C O                   %c\n",186,224,144,186);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c          F A L C U T A D  D E  E S T U D I O S  S U P E R I O R E  S  A C A T L %c N          %c\n",186,181,186);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c              M A T E M %c T I C A S  A P L I C A D A S  Y  C O M P U T A C I %c N              %c\n",186,181,224,186);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c                                    O P T I M I Z A C I %c N                                   %c\n",186,224,186);
    printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c                             R A F A E L  G A R C %c A  J O L L Y                              %c\n",186,214,186);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c                - G A L V %c N  A L V A R E Z  D I E G O                                       %c\n",186,181,186);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c                - M E Z A  C A R R I L L O  G U A D A L U P E                                 %c\n",186,186);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",200,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,188);
	system("pause");
}

void Intro()
{
	system("cls");
	printf("\n\n\t%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",201,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,187);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c                                 M %c T O D O  S I M P L E X                                   %c\n",186,144,186);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",204,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,185);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c       M%ctodo anal%ctico capaz de resolver aquellos modelos que se vuelven complejos en        %c\n",186,130,161,186);
	printf("\t%c       el uso del m%ctodo gr%cafico por el n%cmero de variables empleadas. Este m%ctodo           %c\n",186,130,160,163,130,186);
	printf("\t%c       se ha convertido en el m%ctodo general para resolver problemas de programaci%cn,         %c\n",186,130,162,186);
	printf("\t%c       a diferencia del m%ctodo gr%cfico se puede usar cuando existen m%cs de dos                %c\n",186,130,160,160,186);
	printf("\t%c       variables en el problema.                                                              %c\n",186,186);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c                                                                                              %c\n",186,186);
	printf("\t%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",200,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,188);
}
