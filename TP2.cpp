#include <iostream>

#include <visp/vpDebug.h>
#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpImageSimulator.h>
#include <visp/vpDisplayX.h>
#include <visp/vpHomography.h>
#include <visp/vpDot2.h>


using namespace std ;

void DLT(std::vector<vpImagePoint> vectI1, std::vector<vpImagePoint> vectI2,
                       vpMatrix &aHb)
{
  vpMatrix A(2*vectI1.size(),9,0);
  int ind = 0;
  for(int i = 0; i < 2*vectI1.size(); i=i+2) {
    A[i][3] = -vectI1[ind].get_u();
    A[i][4] = -vectI1[ind].get_v();
    A[i][5] = -1;

    A[i][6] = vectI2[ind].get_v()*vectI1[ind].get_u();
    A[i][7] = vectI2[ind].get_v()*vectI1[ind].get_v();
    A[i][8] = vectI2[ind].get_v();

    A[i+1][0] = vectI1[ind].get_u();
    A[i+1][1] = vectI1[ind].get_v();
    A[i+1][2] = 1;

    A[i+1][6] = -vectI2[ind].get_u()*vectI1[ind].get_u();
    A[i+1][7] = -vectI2[ind].get_u()*vectI1[ind].get_v();
    A[i+1][8] = -vectI2[ind].get_u();

    ind++;
  }

  cout << "A : " << endl;
  cout << A << endl;

  vpColVector w;
  vpMatrix V;
  A.svd(w,V);

  cout << "w : " << endl;
  cout << w << endl;
  cout << "" << endl;

  cout << "V : " << endl;
  cout << V << endl;
  cout << "" << endl;

  int indMinW = 0;
  double minW = 100000000;
  for(int i = 0; i < w.size(); i++) {
    if(w[i] < minW){
      minW = w[i];
      indMinW = i;
    }
  }
  cout << "indMinW = " << indMinW << endl;

  vpColVector hVector(9);
  for(int j = 0; j < hVector.size(); j++) {
    hVector[j] = V[j][indMinW];
  }
  aHb = hVector.reshape(3,3).transpose();
}

vpImagePoint correspondance(vpImagePoint p, vpMatrix H) {
  vpColVector vectP(3,1);
  vectP[0] = p.get_u(); vectP[1] = p.get_v(); 
  vpColVector pointCorrespondant = H*vectP;
  return vpImagePoint(pointCorrespondant[1]/pointCorrespondant[2], 
                      pointCorrespondant[0]/pointCorrespondant[2]);
}

int main()
{
  vpImage<unsigned char> I1;
  vpImage<unsigned char> I2;
  vpImage<vpRGBa> Iimage(876,1200);
  
 
  vpImageIo::read(I1,"../Data/I1.pgm") ;
  vpImageIo::read(I2,"../Data/I2.pgm") ;



  vpCameraParameters cam(800.0, 800.0, 200, 150);
  cam.printParameters() ;

  vpDisplayX d1(I1,10,10,"I1") ;
  vpDisplay::display(I1) ;
  vpDisplay::flush(I1) ;

  vpDisplayX d2(I2,450,10,"I2") ;
  vpDisplay::display(I2) ;
  vpDisplay::flush(I2) ;

  int nb = 5 ;

  // Pour gagner du temps, j'ai mis en "dur" les coordonnées des 5 points dans les 2 images
  vpImagePoint p1[nb], p2[nb];
  p1[0].set_ij(46, 140) ;
  p1[1].set_ij(184, 464) ;
  p1[2].set_ij(423, 430) ;
  p1[3].set_ij(340, 42) ;
  p1[4].set_ij(235, 298) ;

  p2[0].set_ij(130, 122);
  p2[1].set_ij(67, 455);
  p2[2].set_ij(277, 525);
  p2[3].set_ij(390, 200);
  p2[4].set_ij(198, 344);

  std::vector<vpImagePoint> vectI1(5);
  vectI1[0] = p1[0];
  vectI1[1] = p1[1];
  vectI1[2] = p1[2];
  vectI1[3] = p1[3];
  vectI1[4] = p1[4];

  std::vector<vpImagePoint> vectI2(5);
  vectI2[0] = p2[0];
  vectI2[1] = p2[1];
  vectI2[2] = p2[2];
  vectI2[3] = p2[3];
  vectI2[4] = p2[4];

  vpMatrix H(3,3,0);

  DLT(vectI2,vectI1,H);

  
  /*cout << "Notre Homographie: " << endl;
  cout << H << endl;
  cout << "" << endl;

  vpColVector x2(3);
  x2[0] = 122; x2[1] = 130; x2[2] = 1;
  vpColVector x1(3);
  x1 = H*x2;
  x1 = x1/x1[2];
  cout << "x2=(" << x2[1] << "," << x2[0] << ",1), x1 = " << endl;
  cout << x1 << endl;*/

  // cette partie du code calcule la position exacte et précise des 5 centres de gravités des points dans les 2 images. Ce seront vos mesures
    for(unsigned int i=0; i<nb; i++)
      {

	vpDot2 d ;
        d.initTracking(I1,p1[i]) ;
        d.track(I1,p1[i]) ;
	char s[10] ;
	sprintf(s,"%d",i) ;
        vpDisplay::displayCross(I1,p1[i],10,vpColor::blue) ;
        vpDisplay::displayCharString(I1,p1[i],s,vpColor::red) ;
        vpDisplay::flush(I1) ;
      }

    for(unsigned int i=0; i<nb; i++)
      {
        vpDot2 d ;
        d.initTracking(I2,p2[i]) ;
        d.track(I2,p2[i]) ;
	char s[10] ;
	sprintf(s,"%d",i) ;
        vpDisplay::displayCross(I2,p2[i],10,vpColor::green) ;
        vpDisplay::displayCharString(I2,p2[i],s,vpColor::red) ;
        vpDisplay::flush(I2) ;

        
      }
 
  // vous pouvez acceder aux coordonnées des points via les commandes
    // x = p1[i].get_u() ;
    // y = p1[i].get_v()


  // Estimation d'une homographie
   vpMatrix c2Hc1(3,3) ;

  {

        // Calculer l'homographie c2Hc1
        // ..

        cout << "Homographie "<< endl << c2Hc1<< endl ;


        //Verification : transfert de points
         for (int i=0 ; i < nb ; i++)
           {
                // transferer les points p1 dans l'image I2 grâce à l'homographie

                // p'2 = ..  f(p1)  .. à vous de voir ce qu'il y a dans cette equation

                // verifier si p'2 est bien au bon endroit
                // calculer la difference entre la position calculée et la position attendue.

                // tracer un cercle dont le rayon correspondant à l'erreur entre la position calculée et la position attendue.
                // rayon = ...
                // vpDisplay::displayCircle(I2,p'2,rayon,vpColor::green) ; ;
                /*
                vpImagePoint point2;
                vpDisplay::getClick(I2,point2);
                vpDisplay::displayCross(I2,point2,10,vpColor::blue);
                vpDisplay::flush(I2);
                vpImagePoint point1 = correspondance(point2,H);
                cout << point1 << endl;
                vpDisplay::displayCross(I1,point1,10,vpColor::blue);
                vpDisplay::flush(I1);
                */
                vpImagePoint point1 = correspondance(p2[i],H);
                vpDisplay::displayCross(I1,point1,10,vpColor::red);
                float distance = sqrt(pow(point1.get_u()-p1[i].get_u(),2)+pow(point1.get_v()-p1[i].get_v(),2));
                std::cout<<"d :"<<distance<<std::endl;
                vpDisplay::displayCircle(I1,point1,10*distance,vpColor::red);
                vpDisplay::flush(I1);
                std::cout<<i<<std::endl;
            }
  }
  vpImage<unsigned char> I1tmp=I1;
  DLT(vectI1,vectI2,H);
 //TODO: pzrcourir tt image 1 et melangé les points correspondant
  for(int i=0;i<I1.getHeight();i++){
    for(int j=0;j<I1.getWidth();j++){
      //std::cout<<"i : "<<i<<";j :"<<j<<std::endl;
      vpImagePoint p1;
      p1.set_i(i);
      p1.set_j(j);
      vpImagePoint tmp=correspondance(p1,H);
      if(tmp.get_i()>=0 && tmp.get_i()<I2.getHeight() && tmp.get_j()>=0 && tmp.get_j()<I2.getWidth()){
        I1tmp[i][j]=(unsigned char) ((I1[i][j]+I2(tmp))/2);
      }
    }
  }
  vpDisplayX d3(I1tmp,10,10,"I1tmp") ;
  vpDisplay::display(I1tmp);
  vpDisplay::flush(I1tmp) ;

  vpDisplay::flush(I1) ;
  vpDisplay::flush(I2) ;
  vpImage<vpRGBa> Ic ;
  vpDisplay::getImage(I1,Ic) ;
  vpImageIo::write(Ic,"resultat.png") ;

  vpDisplay::getClick(I1) ;

  vpDisplay::close(I2) ;
  vpDisplay::close(I1) ;



  return 0;
}
