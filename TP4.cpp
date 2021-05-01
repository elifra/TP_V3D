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

bool bord = false;

vpImage<float> wta(vpImage<uchar> I1, vpImage<uchar> I2) {
    vpImage<float> res(I1.getRows(),I1.getCols());
    for(int i =0;i<I1.getRows();i++){
        for(int j=0;j<I1.getCols();j++){
            int indiceMin=0;
            int min = 255;
            for(int k=0;k<I2.getCols();k++){
                if(abs((int) (I1[i][j]) - (int) (I2[i][k]))<min){
                    min = abs((int) (I1[i][j]) - (int) (I2[i][k]));
                    indiceMin=k;
                }
            }
            res[i][j]=abs(j-indiceMin);
        }
    }
    return res;
}

vpImage< uchar >  getCell(vpImage< uchar > &I, int i, int j,int taille)
{   
    vpImage< uchar >  res(taille,taille,0);
    // Si le filtre dépasse de l'image au niveau des colonnes ou des lignes on revoie 0
    if(i-(int)(taille/2)<0 || j-(int)(taille/2)<0){
        bord = true;
        return res;
    }
    if(i+(int)(taille/2)>=I.getHeight() || j+(int)(taille/2)>=I.getWidth()){
        bord = true;
        return res;
    }
    else{
        bord = false;
        for(int k=0;k<taille;k++){
            for(int l=0;l<taille;l++){
                //n l'indice de la ligne courante
                int n =i+k-(int)(taille/2);
                //m l'indice de la colonne courante
                int m =j+l-(int)(taille/2);
                res[k][l]=I[n][m];  
            }   
        }
        return res;
    }
}

float compute(vpImage< uchar > cell_I1,vpImage< uchar > cell_I2,vpImage< double > K, int taille){
    double res =0;
    for(int i=0;i<taille;i++){
        for(int j=0;j<taille;j++){
            float tmp = (float) (cell_I1[i][j])- (float) (cell_I2[i][j]);
            res+= K[i][j]*pow(tmp,2);
        }
    }
    return res;
}

vpImage<float> ssd(vpImage<uchar> I1, vpImage<uchar> I2,  vpImage< double >  K) {
    vpImage<float> res(I1.getRows(),I1.getCols());
    for(int i =0;i<I1.getRows();i++){
        for(int j=0;j<I1.getCols();j++){
            int indiceMin=0;
            int min = 255*K.getRows()*K.getCols();
            vpImage< uchar > cell_I1=getCell(I1,i,j,K.getRows());
            if(!bord) {
                for(int k=0;k<I2.getCols();k++){
                    vpImage< uchar > cell_I2=getCell(I2,i,k,K.getRows());
                    if(!bord) {
                        float diff = compute(cell_I1,cell_I2,K,K.getRows());
                        if(diff<min){
                            min = diff;
                            indiceMin=k;
                        }
                    }
                }
            }
            else{
                indiceMin=j;
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
            
            vpImage<float> IwtaF =wta(Iscene_l,Iscene_r);
            vpImage<uchar> Iwta;
            vpImageConvert::convert(IwtaF,Iwta);
            vpDisplayX dwta(Iwta,1000,10,"carte de disparité avec WTA") ;
            vpDisplay::display(Iwta) ;
            vpDisplay::flush(Iwta) ;
            vpDisplay::getClick(Iwta);

            int tailleMasque;
            cout << "Choisissez la taille du masque (1;3;7;21)" << endl;
            cin >> tailleMasque;
            vpImage<double> K(tailleMasque,tailleMasque);
            if(tailleMasque == 1) {
                K[0][0] = 1;
                vpImage<float> IssdF =ssd(Iscene_l,Iscene_r,K);
                vpImage<uchar> Issd;
                vpImageConvert::convert(IssdF,Issd);
                vpDisplayX dssd(Issd,1100,10,"carte de disparité avec SSD") ;
                vpDisplay::display(Issd) ;
                vpDisplay::flush(Issd) ;
                vpDisplay::getClick(Issd);
            }
            else if(tailleMasque == 3) {
                K[0][0] = 1./16.; K[0][1] = 2./16.; K[0][2] = 1./16.; 
                K[1][0] = 2./16.; K[1][1] = 4./16.; K[1][2] = 2./16.;
                K[2][0] = 1./16.; K[2][1] = 2./16.; K[2][2] = 1./16.;
                cout << K << endl;
                vpImage<float> IssdF =ssd(Iscene_l,Iscene_r,K);
                
                //On affiche l'inverse de la disparité 
                for(int i = 0; i < IssdF.getRows(); i++) {
                    for(int j = 0; j < IssdF.getCols(); j++) {
                        if(IssdF[i][j] != 0) IssdF[i][j] = 1./IssdF[i][j];
                        else IssdF[i][j] = 1;
                    }
                }
                vpImage<uchar> Issd;
                vpImageConvert::convert(IssdF,Issd);
                vpDisplayX dssd(Issd,1100,10,"carte de disparité avec SSD") ;
                vpDisplay::display(Issd) ;
                vpDisplay::flush(Issd) ;
                vpDisplay::getClick(Issd);
            }
            else if(tailleMasque == 7) {
                K[0][0] = 1./484.; K[0][1] = 2./484.; K[0][2] = 4./484.; K[0][3] = 8./484.; K[0][4] = 4./484.; K[0][5] = 2./484.; K[0][6] = 1./484.; 
                K[1][0] = 2./484.; K[1][1] = 4./484.; K[1][2] = 8./484.; K[1][3] = 16./484.; K[1][4] = 8./484.; K[1][5] = 4./484.; K[1][6] = 2./484.;
                K[2][0] = 4./484.; K[2][1] = 8./484.; K[2][2] = 16./484.; K[2][3] = 32./484.; K[2][4] = 16./484.; K[2][5] = 8./484.; K[2][6] = 4./484.;
                K[3][0] = 8./484.; K[3][1] = 16./484.; K[3][2] = 32./484.; K[3][3] = 64./484.; K[3][4] = 32./484.; K[3][5] = 16./484.; K[3][6] = 8./484.; 
                K[4][0] = 4./484.; K[4][1] = 8./484.; K[4][2] = 16./484.; K[4][3] = 32./484.; K[4][4] = 16./484.; K[4][5] = 8./484.; K[4][6] = 4./484.;
                K[5][0] = 2./484.; K[5][1] = 4./484.; K[5][2] = 8./484.; K[5][3] = 16./484.; K[5][4] = 8./484.; K[5][5] = 4./484.; K[5][6] = 2./484.;
                K[6][0] = 1./484.; K[6][1] = 2./484.; K[6][2] = 4./484.; K[6][3] = 8./484.; K[6][4] = 4./484.; K[6][5] = 2./484.; K[6][6] = 1./484.;
                vpImage<float> IssdF =ssd(Iscene_l,Iscene_r,K);

                //On affiche l'inverse de la disparité 
                for(int i = 0; i < IssdF.getRows(); i++) {
                    for(int j = 0; j < IssdF.getCols(); j++) {
                        if(IssdF[i][j] != 0) IssdF[i][j] = 1./IssdF[i][j];
                        else IssdF[i][j] = 1;
                    }
                }
                vpImage<uchar> Issd;
                vpImageConvert::convert(IssdF,Issd);
                vpDisplayX dssd(Issd,1100,10,"carte de disparité avec SSD") ;
                vpDisplay::display(Issd) ;
                vpDisplay::flush(Issd) ;
                vpDisplay::getClick(Issd);
            }
            else if(tailleMasque == 21) {
                K[0][0] = 1;
                vpImage<float> IssdF =ssd(Iscene_l,Iscene_r,K);
                vpImage<uchar> Issd;
                vpImageConvert::convert(IssdF,Issd);
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