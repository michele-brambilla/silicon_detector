*********************************************
*     Crea il file "oname" dove verranno messi gli hi
      function init(oname)

      common/pawc/h(99999999)      
      include 'common.inc'
      character*50 oname

      call hlimit(99999999)
      call hropen(2,'analisi',oname,'N',4096,istat)

      call hldir('//','')

      end function init



*********************************************
*     Chiude il file contenente gli istogrammi
      function finalize()

      call hldir('//','T')
      call hcdir('//analisi','')
      call hrout(0,icycle,'')
      call hrend('analisi')
      close(2)

      end function finalize



*********************************************
*     Apre la tupla o restituisce un messaggio di errore
      subroutine apri_tupla(tname,nmax)

      include 'common.inc'
      integer nmax,istat
      character*50 tname

      print *,"file = ",trim(tname(1:21))
      call hropen(40,'dati',tname,'',4096,istat)

      if (istat.eq.0) then
         call hrin(1,99999,0)
         call hbname(1,' ',0,'$clear')
         call hbname(1,'VFASNTUP',IEVENT,'$set')
         call hnoent(1,nmax)
         print *,'n events = ', nmax
      else
         print *,"Impossibile aprire il file",tname
      endif

      call hldir('//','')
      
      end subroutine apri_tupla


*********************************************
*     Chiude la tupla 
      function chiudi_tupla()

*      call hdelet(1)
      call hrend('dati')
      close (40)
            
      end function chiudi_tupla


*********************************************
*     Legge le entrate della tupla corrispondenti al tele1x, basculo e
*     tele2x partendo dall'entrata is+1
      subroutine get_raw_data(v1,v2,v3,is)
      include 'common.inc'

      integer,parameter :: Nstrip=384
      integer :: is
      
      integer index
      real v1(Nstrip)
      real v2(Nstrip)
      real v3(Nstrip)
      
      do istrip=1,Nstrip
         v1(istrip)=IVFAS_DATA1(istrip+is)
         v2(istrip)=IVFAS_DATA3(istrip+is)
         v3(istrip)=IVFAS_DATA5(istrip+is)
      end do
      
      end subroutine get_raw_data


      subroutine get_raw_data6(v,is)
      include 'common.inc'
      integer,parameter :: Nstrip=384
      integer :: is      
      integer index
      real v(Nstrip)
      do istrip=1,Nstrip
         v(istrip)=IVFAS_DATA6(istrip+is)
      end do
      end subroutine get_raw_data6

      subroutine get_raw_data5(v,is)
      include 'common.inc'
      integer,parameter :: Nstrip=384
      integer :: is      
      integer index
      real v(Nstrip)
      do istrip=1,Nstrip
         v(istrip)=IVFAS_DATA5(istrip+is)
      end do
      end subroutine get_raw_data5

      subroutine get_raw_data4(v,is)
      include 'common.inc'
      integer,parameter :: Nstrip=384
      integer :: is      
      integer index
      real v(Nstrip)
      do istrip=1,Nstrip
         v(istrip)=IVFAS_DATA4(istrip+is)
      end do
      end subroutine get_raw_data4

      subroutine get_raw_data3(v,is)
      include 'common.inc'
      integer,parameter :: Nstrip=384
      integer :: is      
      integer index
      real v(Nstrip)
      do istrip=1,Nstrip
         v(istrip)=IVFAS_DATA3(istrip+is)
      end do
      end subroutine get_raw_data3

      subroutine get_raw_data2(v,is)
      include 'common.inc'
      integer,parameter :: Nstrip=384
      integer :: is      
      integer index
      real v(Nstrip)
      do istrip=1,Nstrip
         v(istrip)=IVFAS_DATA2(istrip+is)
      end do
      end subroutine get_raw_data2

      subroutine get_raw_data1(v,is)
      include 'common.inc'
      integer,parameter :: Nstrip=384
      integer :: is      
      integer index
      real v(Nstrip)
      do istrip=1,Nstrip
         v(istrip)=IVFAS_DATA1(istrip+is)
      end do
      end subroutine get_raw_data1

      
      
      subroutine led_event(led)
      include 'common.inc'

      integer :: led
      led=INFO_PLUS(2)

      end subroutine led_event


      subroutine hfill_c(id,x,y,w)
      call hfill(id,x,y,w)
      end subroutine hfill_c

      subroutine hf1_c(id,x)
      call hf1(id,x)
      end subroutine hf1_c
