#include <iostream>
#include <random>
#include <functional>

#include <visp/vpDebug.h>
#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpImageSimulator.h>
#include <visp/vpDisplayX.h>
#include <visp/vpKeyPointSurf.h>
#include <visp/vpHomography.h>
#include <cstdlib>

using namespace std ;

vpImage<uchar> wta(vpImage<uchar> I1, vpImage<uchar> I2) {
    vpImage<uchar> res(I1.getRows(),I1.getCols());
    for(int i =0;i<I1.getRows();i++){
        for(int j=0;j<I1.getCols();j++){
            int indiceMin=0;
            int min = 255;
            for(int k=0;k<I2.getCols();k++){
                if(abs(I1[i][j]-I2[i][k])<min){
                    min = abs(I1[i][j]-I2[i][k]);
                    indiceMin=k;
                }
            }
            res[i][j]=abs(j-indiceMin);
        }
    }
    return res;
}

double convolution(const vpImage< uchar > &I1, int i1, int j1,  const vpImage< uchar > &I2, int i2, int j2,  vpImage< double >  K)
{   
    // Si le filtre dépasse de l'image au niveau des colonnes ou des lignes on revoie 0
    double res=0;
    for(int k=0;k<K.getRows();k++){
        for(int l=0;l<K.getCols();l++){
            int v1 = 0;
            if(i1-(int)(K.getRows()/2)>=0 && j1-(int)(K.getCols()/2)>=0){
                if(i1+(int)(K.getRows()/2)<I1.getHeight() && j1+(int)(K.getCols()/2)<I1.getWidth()){
                    //n l'indice de la ligne courante
                    int n =i1+k-(int)(K.getRows()/2);
                    //m l'indice de la colonne courante
                    int m =j1+l-(int)(K.getCols()/2);
                    v1=I1[n][m];
                }
            }
            int v2 = 0;
            if(i2-(int)(K.getRows()/2)>=0 && j2-(int)(K.getCols()/2)>=0){
                if(i2+(int)(K.getRows()/2)<I2.getHeight() && j2+(int)(K.getCols()/2)<I2.getWidth()){
                    //n l'indice de la ligne courante
                    int n =i2+k-(int)(K.getRows()/2);
                    //m l'indice de la colonne courante
                    int m =j2+l-(int)(K.getCols()/2);
                    v2=I2[n][m];
                }
            }
            res+=K[k][l]*pow(v1-v2,2);  
        }   
    }
    return res;
}

vpImage<uchar> ssd(vpImage<uchar> I1, vpImage<uchar> I2,  vpImage< double >  K) {
    vpImage<uchar> res(I1.getRows(),I1.getCols());
    for(int i =0;i<I1.getRows();i++){
        for(int j=0;j<I1.getCols();j++){
            int indiceMin=0;
            int min = 255*K.getRows()*K.getCols();
            for(int k=0;k<I2.getCols();k++){
                double diff = convolution(I1,i,j,I2,i,k,K);
                if(diff<min){
                     min = diff;
                    indiceMin=k;
                }
            }
            res[i][j]=abs(j-indiceMin);
        }
    }
    return res;
}

int main() {
    vpImage<unsigned char> Ileft;
    vpImage<unsigned char> Iright;
    vpImage<unsigned char> Iscene_l;
    vpImage<unsigned char> Iscene_r;
    vpImage<unsigned char> Icedar_l;
    vpImage<unsigned char> Icedar_r;

    vpImageIo::read(Ileft,"../Data/left.png");
    vpImageIo::read(Iright,"../Data/right.png");
    vpImageIo::read(Iscene_l,"../Data/scene_l.pgm");
    vpImageIo::read(Iscene_r,"../Data/scene_r.pgm");
    vpImageIo::read(Icedar_l,"../Data/cedar-left.jpg");
    vpImageIo::read(Icedar_r,"../Data/cedar-right.jpg");

    while(true) { 
        cout << "CHOISISSEZ PARMIS LES IMAGES SUIVANTES" << endl;
        cout << "1 : left/right" << endl;
        cout << "2 : scene_l/scene_r" << endl;
        cout << "3 : cedar-left/cedar-right" << endl;
        cout << "0 : quitter" << endl;

        int choix;
        cin >> choix;

        if(choix == 1) {
            vpDisplayX dg(Ileft,10,10,"Ileft") ;
            vpDisplay::display(Ileft) ;
            vpDisplay::flush(Ileft) ;
            vpDisplay::getClick(Ileft);

            vpDisplayX dd(Iright,450,10,"Iright") ;
            vpDisplay::display(Iright) ;
            vpDisplay::flush(Iright) ;
            vpDisplay::getClick(Iright);
            std::cout<<"Mais c'est pas rectifié frère"<<std::endl;
            
        }
        else if(choix == 2) {
            vpDisplayX dg(Iscene_l,10,10,"Iscene_l") ;
            vpDisplay::display(Iscene_l) ;
            vpDisplay::flush(Iscene_l) ;
            vpDisplay::getClick(Iscene_l);

            vpDisplayX dd(Iscene_r,450,10,"Iscene_r") ;
            vpDisplay::display(Iscene_r) ;
            vpDisplay::flush(Iscene_r) ;
            vpDisplay::getClick(Iscene_r);
            
            std::cout<<"Niquel frère, c'est rectifié ça ! j'y mettrais ma main au feu"<<std::endl;
            
            vpImage<uchar> Iwta =wta(Iscene_l,Iscene_r);
            vpDisplayX dwta(Iwta,1000,10,"carte de disparité avec WTA") ;
            vpDisplay::display(Iwta) ;
            vpDisplay::flush(Iwta) ;
            vpDisplay::getClick(Iwta);

            int tailleMasque;
            cout << "Choisissez la taille du masque" << endl;
            cin >> tailleMasque;
            vpImage<double> K(tailleMasque,tailleMasque);
            if(tailleMasque == 1) {
                K[0][0] = 1;
                vpImage<uchar> Issd =ssd(Iscene_l,Iscene_r,K);
                vpDisplayX dssd(Issd,1100,10,"carte de disparité avec SSD") ;
                vpDisplay::display(Issd) ;
                vpDisplay::flush(Issd) ;
                vpDisplay::getClick(Issd);
            }
            else if(tailleMasque == 3) {
                K[0][0] = 1/16; K[0][1] = 2/16; K[0][2] = 1/16; 
                K[1][0] = 2/16; K[1][1] = 4/16; K[1][2] = 2/16;
                K[2][0] = 1/16; K[2][1] = 2/16; K[2][2] = 1/16;
                vpImage<uchar> Issd =ssd(Iscene_l,Iscene_r,K);
                vpDisplayX dssd(Issd,1100,10,"carte de disparité avec SSD") ;
                vpDisplay::display(Issd) ;
                vpDisplay::flush(Issd) ;
                vpDisplay::getClick(Issd);
            }
            else if(tailleMasque == 7) {
                K[0][0] = 1;
                vpImage<uchar> Issd =ssd(Iscene_l,Iscene_r,K);
                vpDisplayX dssd(Issd,1100,10,"carte de disparité avec SSD") ;
                vpDisplay::display(Issd) ;
                vpDisplay::flush(Issd) ;
                vpDisplay::getClick(Issd);
            }
            else if(tailleMasque == 20) {
                K[0][0] = 1;
                vpImage<uchar> Issd =ssd(Iscene_l,Iscene_r,K);
                vpDisplayX dssd(Issd,1100,10,"carte de disparité avec SSD") ;
                vpDisplay::display(Issd) ;
                vpDisplay::flush(Issd) ;
                vpDisplay::getClick(Issd);
            }
        }
        else if(choix == 3) {
            vpDisplayX dg(Icedar_l,10,10,"Icedar_l") ;
            vpDisplay::display(Icedar_l) ;
            vpDisplay::flush(Icedar_l) ;
            vpDisplay::getClick(Icedar_l);

            vpDisplayX dd(Icedar_r,450,10,"Icedar_r") ;
            vpDisplay::display(Icedar_r) ;
            vpDisplay::flush(Icedar_r) ;
            vpDisplay::getClick(Icedar_r);
            std::cout<<"Mais c'est pas rectifié frère"<<std::endl;

        }
        else if(choix == 0) break;
        else cout << "Numéro invalide" << endl;
    }
    
    return 0;
}