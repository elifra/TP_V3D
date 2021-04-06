#include <iostream>

#include <visp/vpDebug.h>
#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpImageSimulator.h>
#include <visp/vpDisplayX.h>


using namespace std ;



int main()
{
  vpImage<unsigned char> I1(300,400,0);
  vpImage<unsigned char> I2(300,400,0);
  vpImage<vpRGBa> Iimage(876,1200);
  
 
  vpImageIo::read(Iimage,"../Data/right.png") ;
  
// Cette partie ne sert qu'a la simulation
  // on positionne un poster dans le repere Rw

  double L = 0.600 ;
  double l = 0.438;
  // Initialise the 3D coordinates of the Iimage corners
  vpColVector X[4];
  for (int i = 0; i < 4; i++) X[i].resize(3);
  // Top left corner
  X[0][0] = -L;
  X[0][1] = -l;
  X[0][2] = 0;
  
  // Top right corner
  X[1][0] = L;
  X[1][1] = -l;
  X[1][2] = 0;
  
  // Bottom right corner
  X[2][0] = L;
  X[2][1] = l;
  X[2][2] = 0;
  
  //Bottom left corner
  X[3][0] = -L;
  X[3][1] = l;
  X[3][2] = 0;
  


  vpImageSimulator sim;
  sim.init(Iimage, X);

  // On définit une camera avec certain parametre u0 = 200, v0 = 150; px = py = 800
  vpCameraParameters cam(800.0, 800.0, 200, 150);
  cam.printParameters() ;

  vpMatrix K = cam.get_K() ;

  cout << "Matrice des paramètres intrinsèques" << endl ;
  cout << K << endl ;
  cout << "" << endl;

  vpMatrix inverseK = cam.get_K_inverse();
  cout << "Inverse de K" << endl;
  cout << inverseK << endl;
  
  // On positionne une camera c1 à la position c1Tw (ici le repere repère Rw est 2m devant Rc1
  vpHomogeneousMatrix  c1Tw(0,0,2,  vpMath::rad(0),vpMath::rad(0),0) ;
  //on simule l'image vue par c1
  sim.setCameraPosition(c1Tw);
  // on recupère l'image
  sim.getImage(I1,cam);
  cout << "Image I1g " <<endl ;
  cout << c1Tw << endl ;

  // On positionne une camera c2 à la position c2Tw
  vpHomogeneousMatrix c2Tw(0,0,1.8, vpMath::rad(0),vpMath::rad(0),0) ;
  //on simule l'image vue par c2
  sim.setCameraPosition(c2Tw);
  // on recupère l'image I2
  sim.getImage(I2,cam);
  cout << "Image I1d " <<endl ;
  cout << c2Tw << endl ;

  vpHomogeneousMatrix c1T2 = c1Tw*c2Tw.inverse();
 
  vpRotationMatrix R = c1T2.getRotationMatrix();

  vpMatrix m_1t2x = c1T2.getTranslationVector().skew();
  cout << "Matrice m_1t2x" << endl;
  cout << m_1t2x << endl; 

  vpMatrix m_1F2 = inverseK.transpose()*m_1t2x*R*inverseK;
  cout << "Matrice fondamentale" << endl;
  cout << m_1F2 << endl;

  //Calcul droite épipolaire
  vpColVector p1(3,0);
  p1[0] = 150; p1[1] = 100; p1[2] = 1;
  vpColVector p2(3,0);
  p2[0] = 50; p2[1] = 75; p2[2] = 1;

  cout << "équation de la droite liée à p1" << endl;
  vpColVector droite1 = m_1F2*p1;
  cout << droite1 << endl;

  cout << "équation de la droite liée à p2" << endl;
  vpColVector droite2 = m_1F2*p2;
  cout << droite2 << endl;
/*
  // On affiche l'image I1
  vpDisplayX d1(I1,10,10,"I1") ;
  vpDisplay::display(I1) ;
  vpDisplay::flush(I1) ;
*/
  //droite sur image I1
  vpDisplayX d1_droite(I1,10,10,"I1 avec droite") ;
  vpDisplay::display(I1) ;
  vpDisplay::displayLine(I1,-droite1[2]/droite1[1],0,-(droite1[2]+I1.getWidth()*droite1[0])/droite1[1],I1.getWidth(),vpColor::red,1,false);
  vpDisplay::displayLine(I1,-droite2[2]/droite2[1],0,-(droite2[2]+I1.getWidth()*droite2[0])/droite2[1],I1.getWidth(),vpColor::blue,1,false);
  vpDisplay::flush(I1) ;

  //Point sur I2

  // On affiche l'image I2
  vpDisplayX d2(I2,10,400,"I2") ;
  vpDisplay::display(I2) ;
  vpDisplay::displayCross(I2,200,100,5,vpColor::red);
  vpDisplay::displayCross(I2,75,50,5,vpColor::blue);
  vpDisplay::flush(I2) ;





  for (int i=0 ; i < 5 ; i++)
    {

      // On recupere les coordonne uv2 d'un point dans l'image 2
      cout << "Click point number " << i << " in I2" << endl ;
      vpImagePoint x2 ;
      vpDisplay::getClick(I2, x2) ;
      
      // on place les coordonné de x2 dans un vecteur (en coordonnées homogène)
      vpColVector uv2(3) ;
      uv2[0] = x2.get_u() ;
      uv2[1] = x2.get_v() ;
      uv2[2] = 1 ;
      
      // Affichage du poin clické
      vpDisplay::displayCross(I2,x2,5,vpColor::red) ;
      vpDisplay::flush(I2) ;



     //....


      // et afficher la droite passant par ces deux points
     // vpDisplay::displayLine(I1,pa,pb,vpColor::red) ;

      vpDisplay::flush(I1) ;
    }



    vpImage<vpRGBa> Ic ;
    vpDisplay::getImage(I1,Ic) ;
    vpImageIo::write(Ic,"I1.jpg") ;
    vpDisplay::getImage(I2,Ic) ;
    vpImageIo::write(Ic,"I2.jpg") ;


  vpDisplay::getClick(I2) ;
  cout << "OK " << endl ;

  vpDisplay::close(I2) ;
  vpDisplay::close(I1) ;


  

  return 0;
}
