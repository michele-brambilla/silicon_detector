********************************
*     prepara gli histo

      subroutine prepara_histo_si()

      include 'variabili.inc'   

      do isilicio=1,Nsilicio

*     pull e PH (per telescopio x-y e per strip)
         call hbook1(isilicio*100000,'PH',1000,0.5,5000.5,0.)
         call hbook1(isilicio*100000+1,'Pull',1000,0.5,1000.5,0.)

         if(.NOT.(isilicio.eq.3)) then
cc            call hbook1(isilicio*100000+10,'prof taglio',384*4,-1.5,5.5
cc     $           ,0.)
cc            call hbook1(isilicio*100000+11,'prof ph',384*4,-1.5,5.5,0.)

            call hbook1(isilicio*100000+10,'prof taglio',384*4,0.,7.,0.)
            call hbook1(isilicio*100000+11,'prof ph',384*4,0.,7.,0.)


         else
            call hbook1(isilicio*100000+10,'prof taglio',384,0.,7.,0.)
            call hbook1(isilicio*100000+11,'prof ph',384,0.,7.,0.)
         endif
         call hbook1(isilicio*100000+12,'phmax',500,0.5,2000.5,0.)

         
         call hbook1(isilicio*100000+113,'interstrip position',50,0.,50.
     $        ,0.)
         call hbook1(isilicio*100000+13,'interstrip position',50,0.,50.
     $        ,0.)

         call hbook1(isilicio*100000+14,'interstrip n-corretta position'
     $        ,50,0.,50.,0.)

         call hbook1(isilicio*100000+114
     $        ,'interstrip n-corretta position',50,0.,50.,0.)

*     pullL, pullR e ETA (per telescopio x-y e per strip)
         call hbook1(isilicio*100000+2,'PullL',1000,0.5,1000.5,0.)
         call hbook1(isilicio*100000+3,'PullR',1000,0.5,1000.5,0.)
         call hbook1(isilicio*100000+4,'eta',225,-1.,1.,0.)


*     cluster e osservabili (per telescopio x-y e per strip)

            call hbook1(isilicio*100000+5,'nr Strip-cluster',20,0.5,20.5
     $           ,0.)
            call hbook1(isilicio*100000+6,'ampiezza-cluster',1000,0.5
     $           ,5000.5,0.)
            if( .not.(isilicio.eq.3) ) then  
               call hbook1(isilicio*100000+7,'baricentro-cluster',100,0.
     $              ,2.,0.)

            else
               call hbook1(isilicio*100000+7,'baricentro-cluster',350,0.
     $              ,7.,0.)

            endif
            call hbook1(isilicio*100000+8,'SNR-cluster',1000,0.5,3000.5
     $           ,0.)
            call hbook1(isilicio*100000+9,'Pull-cluster',1000,0.5,1500.5
     $           ,0.)

      enddo



      
*     PH e pull 2D per basculo per le 20 zone
      call hbook2(310000,'PH-2d',20,0.5,20.5,1500,0.5,1500.5,0.)
      call hbook2(310001,'Pull-2d',20,0.5,20.5,1500,0.5,1500.5,0.)
      
*     pullL, pullR e ETA per basculo per le 20 zone
      call hbook2(310002,'PullL-2d',20,0.5,20.5,1500,0.5,1500.5,0.)
      call hbook2(310003,'PullR-2d',20,0.5,20.5,1500,0.5,1500.5,0.)
      call hbook2(310004,'eta-2d',20,0.5,20.5,225,-1.,1.,0.)
         
*     cluster e osservabili (per basculo per le 20 zone)
      call hbook2(310005,'nr Strip-cluster',20,.5,20.5,20,0.5,20.5,0.)
      call hbook2(310006,'ampiezza-cluster',20,.5,20.5,1000,0.5,5000.5
     $     ,0.) 
      call hbook2(310007,'baricentro-cluster',20,.5,20.5,100,0.,7.,0.)
      call hbook2(310008,'SNR-cluster',20,.5,20.5,1000,0.5,3000.5,0.)
      call hbook2(310009,'Pull-cluster',20,.5,20.5,1000,0.5,1500.5,0.)

      call hbook2(310011,'pull-nocut-2d',20,0.5,20.5,1500,0.5,1500.5,0.)
      call hbook2(310012,'PHmax-2d',20,0.5,20.5,500,0.5,2000.5,0.)

      call hbook2(310013,'interstrip position-2D',20,.5,20.5,50,0.,240.
     $     ,0.)
      call hbook2(310014,'interstrip n-corretta-2D',20,.5,20.5,50,0.
     $     ,240.,0.)

      call hbook2(310113,'interstrip position-2D',20,.5,20.5,50,0.,240.
     $     ,0.)
      call hbook2(310114,'interstrip n-corretta-2D',20,.5,20.5,50,0.
     $     ,240.,0.)

      
      call hbook2(310015,'feta-2D',20,.5,20.5,225,0.
     $     ,1.,0.)
      

*******************************
*     varibili in comune
      
*     beta
      call hbook1(1001,'betax_I',1000,-0.05,0.05,0.)
      call hbook1(1002,'betay_I',1000,-0.05,0.05,0.)

      call hbook1(11001,'betax_II',1000,-0.05,0.05,0.)
      call hbook1(11002,'betay_II',1000,-0.05,0.05,0.)


      
*     diff      
      call hbook1(1003,'diffx',1000,-.5,.5,0.)

      call hbook2(1103,'residui I',20,0.5,20.5,1000,-5.,5.,0.)
      call hbook2(1113,'residui bin-fitto',20,0.5,20.5,600,-0.3,0.3,0.)
      call hbook2(11003,'residui II',20,0.5,20.5,1000,-5.,5.,0.)
      call hbook2(11103,'resid bin-fitto',20,0.5,20.5,600,-0.3,0.3,0.)

      
**************RESIDUIvsINTERSRIP
      do izone=1,20
         call hbook2(3000+izone,'RESIDUIvsINTERSRIP',50,0.,240.,600,
     $        -0.3,0.3,0.)
      enddo
      
      call hbook1(1101,'betax',1000,-0.05,0.05,0.)

*      call hbook1(1068,'ph',
*     $     vec_ROzone(1,1)-vec_ROzone(20,2), ! quanti bin
*     $     float(vec_ROzone( 1,1))+.5, ! inizio
*     $     float(vec_ROzone(20,2))+.5, ! fine
*     $     0.)
*      call hbook1(1069,'soglia',
*     $     vec_ROzone(1,1)-vec_ROzone(20,2), ! quanti bin
*     $     float(vec_ROzone( 1,1))+.5, ! inizio
*     $     float(vec_ROzone(20,2))+.5, ! fine
*     $     0.)

      call hbook1(1068,'ph',
     $     384, ! quanti bin
     $     .5, ! inizio
     $     384.5, ! fine
     $     0.)
      call hbook1(1069,'soglia',
     $     384, ! quanti bin
     $     .5, ! inizio
     $     384+.5, ! fine
     $     0.)

      
      end subroutine prepara_histo_si
