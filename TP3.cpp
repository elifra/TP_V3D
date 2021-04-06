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

/**
 * @brief initialise les points mis en correspondance
 *
 * @param p1: tableau de taille 17
 * @param p2: tableau de taille 17
 */
void getCorrespondances(vpImagePoint * p1, vpImagePoint * p2)
{
    p1[0].set_u(117.5130997);    p1[0].set_v(62.34123611);    p2[0].set_u(202.841095);     p2[0].set_v(36.29648209);
    p1[1].set_u(84.06044006);    p1[1].set_v(67.55551147);    p2[1].set_u(169.5350189);    p2[1].set_v(26.80556679);
    p1[2].set_u(80.27194214);    p1[2].set_v(111.0672302);    p2[2].set_u(147.9641113);    p2[2].set_v(64.5475769);
    p1[3].set_u(342.6855164);    p1[3].set_v(199.8661346);    p2[3].set_u(63.4621048);     p2[3].set_v(68.28819275);
    p1[4].set_u(302.6676636);    p1[4].set_v(226.6687317);    p2[4].set_u(300.4017639);    p2[4].set_v(263.6835022);
    p1[5].set_u(101.5870972);    p1[5].set_v(63.0242424);     p2[5].set_u(187.8421478);    p2[5].set_v(29.56011963);
    p1[6].set_u(153.4119415);    p1[6].set_v(91.05652618);    p2[6].set_u(222.968277);     p2[6].set_v(77.2434845);
    p1[7].set_u(190.6780548);    p1[7].set_v(110.7231598);    p2[7].set_u(247.8312683);    p2[7].set_v(110.4263763);
    p1[8].set_u(302.8087463);    p1[8].set_v(133.9337616);    p2[8].set_u(339.9194641);    p2[8].set_v(178.880661);
    p1[9].set_u(162.7279968);    p1[9].set_v(276.4970398);    p2[9].set_u(152.7050171);    p2[9].set_v(248.9367065);
    p1[10].set_u(151.0850067);   p1[10].set_v(36.12360764);   p2[10].set_u(244.672287);    p2[10].set_v(25.44586563);
    p1[11].set_u(171.7740173);   p1[11].set_v(53.67162704);   p2[11].set_u(256.0083618);   p2[11].set_v(49.99362183);
    p1[12].set_u(116.7895355);   p1[12].set_v(74.19098663);   p2[12].set_u(196.8202972);   p2[12].set_v(45.97808456);
    p1[13].set_u(104.2023163);   p1[13].set_v(83.85998535);   p2[13].set_u(181.4200439);   p2[13].set_v(50.26084518);
    p1[14].set_u(84.71365356);   p1[14].set_v(190.8507233);   p2[14].set_u(300.4017639);   p2[14].set_v(263.6835022);
    p1[15].set_u(138.8526764);   p1[15].set_v(273.5761719);   p2[15].set_u(131.6974182);   p2[15].set_v(236.8515778);
    p1[16].set_u(167.2081451);   p1[16].set_v(96.59983063);   p2[16].set_u(233.1238556);   p2[16].set_v(88.96112061);
}

/**
 * @brief affiche image des appariements entre points de deux images
 *
 * @param I1: image gauche
 * @param I2: image droite
 * @param I: image résultat
 * @param p1: points dans l'image 1
 * @param p2: points dans l'image 2
 * @param nb: nombre de points
 */
void AfficheAppariement(vpImage<unsigned char> &I1,
        vpImage<unsigned char> &I2,
        vpImage<unsigned char> &I,
        vpImagePoint * p1,
        vpImagePoint * p2,
        unsigned int nb)

{

    for (int i =0 ; i < I1.getRows() ; i++)
        for (int j = 0 ; j < I1.getCols() ; j++)
        {
            I[i][j] = I1[i][j] ;
            I[i][j+I1.getCols()] = I2[i][j] ;
        }
    vpDisplay::display(I) ;

    vpDisplay::flush(I) ;


    for(unsigned int i=0; i<nb; i++)
    {
        char s[10] ;
        sprintf(s,"%d",i) ;
        p2[i].set_u(p2[i].get_u()+I1.getCols()) ;
        vpDisplay::displayCharString(I,p1[i],s,vpColor::red) ;
        vpDisplay::displayCharString(I,p2[i],s,vpColor::red) ;
        vpDisplay::displayLine(I,p1[i],p2[i],vpColor::yellow) ;

    }

    vpDisplay::flush(I) ;
}

unsigned char interpole(vpImage<unsigned char> const& img, double i, double j)
{
    double percentI = i - floor(i);
    double percentJ = j - floor(j);

    int N = (int) floor(i);
    int S = N+1;
    int W = (int) floor(j);
    int E = W+1;

    if(        N < 0 || N >= img.getHeight()
            || S < 0 || S >= img.getHeight()
            || W < 0 || W >= img.getWidth()
            || E < 0 || E >= img.getWidth())
        return 0;

    unsigned char valN = (unsigned char) (percentJ * (double) img[N][E] + (1-percentJ) * (double) img[N][W]);
    unsigned char valS = (unsigned char) (percentJ * (double) img[S][E] + (1-percentJ) * (double) img[S][W]);

    return (unsigned char) (percentI * (double) valS + (1-percentI) * (double) valN);
}

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

  //cout << "A : " << endl;
  //cout << A << endl;

  vpColVector w;
  vpMatrix V;
  A.svd(w,V);
/*
  cout << "w : " << endl;
  cout << w << endl;
  cout << "" << endl;

  cout << "V : " << endl;
  cout << V << endl;
  cout << "" << endl;
*/
  int indMinW = 0;
  double minW = 100000000;
  for(int i = 0; i < w.size(); i++) {
    if(w[i] < minW){
      minW = w[i];
      indMinW = i;
    }
  }
  //cout << "indMinW = " << indMinW << endl;

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

void getRandomSample(std::vector<vpImagePoint>& vectI1, std::vector<vpImagePoint>& vectI2){
    int nb=17;
    for(int i =0;i<12;i++){
        int rn=rand()%(nb-i);
        vectI1.erase(vectI1.begin()+rn);
        vectI2.erase(vectI2.begin()+rn);
    }
}

void ransac(std::vector<vpImagePoint> vectI1, std::vector<vpImagePoint> vectI2, vpMatrix & aHb){
    bool pasInsane = true;
    int i=0;
    std::vector<std::pair<vpMatrix,int>> mapBon;
    float n = log10(1-0.95);
    float tmppow =pow((float) (15.0/17.0),5.0);
    n= n/log10(1.0-tmppow);
    ///(log10(1-pow(1-(double)(2/17),5))));
    std::cout<<"ICI LA LE N :"<<n<<endl;
    while(pasInsane && i<(int) n){
        //std::cout << "i = " << i << std::endl;
        std::vector<vpImagePoint> sampleVectI1=vectI1;
        std::vector<vpImagePoint> sampleVectI2=vectI2;
        getRandomSample(sampleVectI1,sampleVectI2);

        vpMatrix H(3,3,0);
        DLT(sampleVectI2,sampleVectI1,H);

        int bon=0;
        for(int j=0;j<17;j++){
            vpImagePoint tmp =correspondance(vectI2[j],H);
            float distance = sqrt(pow(tmp.get_u()-vectI1[j].get_u(),2)+pow(tmp.get_v()-vectI1[j].get_v(),2));
            if(distance<2){
                bon++;
            }
        }
        //std::cout<<"bon :"<<bon<<std::endl;
        mapBon.push_back(std::pair<vpMatrix,int>(H,bon));
        if(bon==17) pasInsane=false;
        i++;
    } 
    int max=0;
    for(std::pair<vpMatrix,int> p : mapBon){
        if(p.second>max){
            max = p.second;
            aHb=p.first;
        }
    }
}





int main()
{
    vpImage<unsigned char> I1(300,400,0);
    vpImage<unsigned char> I2(300,400,0);
    vpImage<vpRGBa> Iimage(876,1200);


    vpImageIo::read(Iimage,"../Data/big-sleep.jpg") ;


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
    vpCameraParameters cam(800.0, 800.0, 200, 150);

    cam.printParameters() ;

    // I1g
    vpHomogeneousMatrix  c1gMo(0,0,2,  vpMath::rad(0),vpMath::rad(0),0) ;
    sim.setCameraPosition(c1gMo);
    sim.getImage(I1,cam);
    cout << "Image I1g " <<endl ;
    cout << c1gMo << endl ;

    // I1d
    vpHomogeneousMatrix c1dMo(0.1,0,2,
            vpMath::rad(0),vpMath::rad(0),vpMath::rad(25)) ; //0.1,0,2, vpMath::rad(0),vpMath::rad(0),0) ;
    sim.setCameraPosition(c1dMo);
    sim.getImage(I2,cam);
    cout << "Image I1d " <<endl ;
    cout << c1dMo << endl ;

    vpHomogeneousMatrix cgMcd = c1gMo * c1dMo.inverse() ;
    vpMatrix K = cam.get_K() ;

    vpDisplayX dg(I1,10,10,"I1") ;
    vpDisplay::display(I1) ;
    vpDisplay::flush(I1) ;

    vpDisplayX dd(I2,450,10,"I2") ;
    vpDisplay::display(I2) ;
    vpDisplay::flush(I2) ;

    // Image resultat
    vpImage<unsigned char> I ;
    I.resize(I1.getRows(), I1.getCols()*2) ;
    vpDisplayX d(I,10,400,"I") ;

    //vpKeyPointSurf surf;
    //surf.setDescriptorType(vpKeyPointSurf::extendedDescriptor) ;
    //First grab the reference image Irefrence
    //surf.setHessianThreshold(10000) ;

    //Build the reference SURF points.
    //surf.buildReference(I1);

    //Then grab another image which represents the current image Icurrent
    vpDisplay::flush(I1) ;
    //Display the matched points
    //surf.display(I1, I2);
    int nb = 17;
    vpImagePoint p1[nb], p2[nb];
    getCorrespondances(p1, p2);
    AfficheAppariement(I1,  I2, I, p1, p2, nb);

    std::vector<vpImagePoint> vectI1(nb);
    std::vector<vpImagePoint> vectI2(nb);
    for(int i = 0; i < nb; i++) {
        vectI1[i] = p1[i];
        vectI2[i] = p2[i];
    } 

    vpMatrix H(3,3,0);
    //DLT(vectI2,vectI1,H);
    ransac(vectI1,vectI2,H);
    for (int i=0 ; i < nb ; i++)
           {
                vpImagePoint point1 = correspondance(p2[i],H);
                vpDisplay::displayCross(I1,point1,10,vpColor::red);
                vpDisplay::displayLine(I1,point1,p1[i],vpColor::blue);
                vpDisplay::flush(I1);
                //std::cout<<i<<std::endl;
            }


    /*if(nb >= 0){ // ... add paired points to vectPts

        for(unsigned int i=0; i<nb; i++)
        {
            //surf.getMatchedPoints(i, p1[i], p2[i]);
            char s[10] ;
            sprintf(s,"%d",i) ;
            cout << i <<"  "  << p1[i].get_u() <<"  " << p1[i].get_v() <<"  " ;
            cout <<  p2[i].get_u() <<"  " << p2[i].get_v() << endl;
            vpDisplay::displayCharString(I1,p1[i],s,vpColor::yellow) ;
            vpDisplay::displayCharString(I2,p2[i],s,vpColor::yellow) ;

            vpDisplay::flush(I1);
            vpDisplay::flush(I2);
        }
    }*/


    // Calcul homography et erreur de reprojetion


    // cf TP2 augmenté

    vpDisplay::flush(I1);
    vpDisplay::flush(I2);

    vpDisplay::getClick(I);

    return 0;
}
