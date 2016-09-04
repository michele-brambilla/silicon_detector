****  ESC-X fortran-mode

      common /rawdata/raw
      common /data/subraw,imax
      common /cuts/pitch,x_conv,cut1,cut2
      common /xcluster/nrcluster,baricentro_mc,ampiezza_mc,nrS
      common /xpede/subpede,subrms,istat,istatus,xavg
      common /efficenza/trk_tot,trk_buone,trk_fallite
      common /angoli/betax,betay,angolo
      
*      parameter (start=4) ! le strip dalla V in avanti x Si
      integer,parameter :: nasic=3
      integer,parameter :: nchan=128
      integer,parameter :: nasic_b=5
      integer,parameter :: nchan_b=64
      integer,parameter :: Nstrip=384
      integer,parameter :: Nsilicio=5
      integer,parameter :: Maxcluster=20
      real,parameter :: sigmax=0.1358
      real,parameter :: sigmay=0.1029

      character*50 output_dir

      integer istatus(Nstrip,Nsilicio)
      integer nrS(Nsilicio)

      real raw(Nstrip,Nsilicio), subraw(Nstrip,Nsilicio) ! generali
      real subpede(Nstrip,Nsilicio),subrms(Nstrip,Nsilicio)
      ! taglio principale sul pull
      real pitch(Nstrip,Nsilicio),x_conv(Nstrip
     $     ,Nsilicio),cut1(Nsilicio)       

      real betax,betay,angolo

      
      real xavg(Nstrip,Nsilicio) ! equalizzazione
      

      integer imax(Nsilicio)

      real cut2(Nsilicio)       ! taglio pull L/R

      real baricentro_mc(Nsilicio,Maxcluster) ! cluster usando baricentro

      real ampiezza_mc(Nsilicio,Maxcluster) ! per efficienza

      integer nrcluster(Nsilicio)

      integer istat ! varie

      integer trk_tot,trk_buone,trk_fallite ! efficenza

*****************
     
      integer lastevent
      integer do_display         ! dice di fare il displayevent
      
      common/display/do_display

***********************************************


      
**************basculo*****************
      integer vec_ROzone(20,2),zone(5,2),nrFloating(20)
      integer zonaI,zonaF,primastrip_b,ultimastrip_b,zona_segnale
      real x_i,x_f,pitch_b(20)
      common/basculo/vec_ROzone,zone,nrFloating,zonaI,zonaF,x_i,x_f
     $     ,pitch_b,primastrip_b,ultimastrip_b,zona_segnale

**************eta_correction*****************
      real eta(Nsilicio+20)
      
      real eta_t(5,225)
      real eta_b(20,225)
      real cut2_t(Nsilicio),cut1_b(20),cut2_b(20)
      real xpos(Nsilicio,Maxcluster)
      common/eta_corr/eta,eta_t,eta_b,xpos,cut2_t,cut1_b,cut2_b
      
*************align***************************
      real align_x2,align_x13
      common/alignment/align_x2,align_x13
