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
vpImage<uchar> ssd(vpImage<uchar> I1, vpImage<uchar> I2) {
    vpImage<uchar> res(I1.getRows(),I1.getCols());
    for(int i =0;i<I1.getRows();i++){
        for(int j=0;j<I1.getCols();j++){
            int indiceMin=0;
            int min = 255;
            for(int k=0;k<I2.getCols();k++){
                //TODO: faire le truc de minimisation avec une fonction auxiliaire boloss
                //TODO calculer la valeur avec la nouvelle fonction
                float untruc;
                if(untruc<min){
                     min = untruc;
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