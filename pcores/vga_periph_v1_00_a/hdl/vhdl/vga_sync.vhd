-------------------------------------------------------------------------------
--  Department of Computer Engineering and Communications                     
--  Author: LPRS2  <lprs2@rt-rk.com>                                          
--                                                                            
--  Module Name: vga_sync                                                     
--                                                                            
--  Description:                                                              
--                                                                            
--    Implementation of VGA synchronization
--                                                                            
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity vga_sync is 
  generic (
    HORIZONTAL_RES : natural := 800;
    VERTICAL_RES   : natural := 600;
    FRAME_SIZE     : natural := 4
  );
  port (
    clk_i               : in  std_logic;
    rst_n_i             : in  std_logic;
    show_frame_i        : in  std_logic;
    --
    direct_mode_i       : in  std_logic; -- 0 - text and graphics interface mode, 1 - direct mode (direct force RGB component)
    dir_red_i           : in  std_logic_vector(7 downto 0);
    dir_green_i         : in  std_logic_vector(7 downto 0);
    dir_blue_i          : in  std_logic_vector(7 downto 0);
    --
    active_pixel_i      : in  std_logic;
    foreground_color_i  : in  std_logic_vector(23 downto 0);
    background_color_i  : in  std_logic_vector(23 downto 0);
    frame_color_i       : in  std_logic_vector(23 downto 0);
    red_o               : out std_logic_vector(7 downto 0);
    green_o             : out std_logic_vector(7 downto 0);
    blue_o              : out std_logic_vector(7 downto 0);
    pixel_row_o         : out std_logic_vector (10 downto 0);
    pixel_column_o      : out std_logic_vector (10 downto 0);
    horiz_sync_o        : out std_logic;
    vert_sync_o         : out std_logic;
    psave_o             : out std_logic;
    blank_o             : out std_logic;
    pix_clk_o           : out std_logic;
    sync_o              : out std_logic
  );
  end vga_sync;

architecture rtl of vga_sync is

  signal horiz_sync_r       : std_logic;
  signal vert_sync_r        : std_logic;
  signal enable_s           : std_logic;
  signal h_count_r          : std_logic_vector( 10 downto 0 );
  signal v_count_r          : std_logic_vector( 10 downto 0 );
  
  signal horiz_sync_out_d_r : std_logic;
  signal vert_sync_out_d_r  : std_logic;
  signal psave_d_r          : std_logic;
  signal blank_d_r          : std_logic;
  signal sync_d_r           : std_logic;
  
  -- signali za registrovanje izlaza
  signal red_r              : std_logic_vector(7 downto 0);
  signal green_r            : std_logic_vector(7 downto 0);
  signal blue_r             : std_logic_vector(7 downto 0);
  signal horiz_sync_out_r   : std_logic;
  signal vert_sync_out_r    : std_logic;
  signal pixel_row_r        : std_logic_vector(10 downto 0);
  signal pixel_column_r     : std_logic_vector(10 downto 0);
  signal psave_r            : std_logic;
  signal blank_r            : std_logic;
  signal sync_r             : std_logic;
  
   -- konstatne horizontalne sinhronizacije
  signal  h_pixels          : integer range 0 to 2047;
  signal  h_frontporch      : integer range 0 to 2047;
  signal  h_sync_time       : integer range 0 to 2047;
  signal  h_backporch       : integer range 0 to 2047;
  
   -- konstatne vertikalne sinhronizacije
  signal  v_lines           : integer range 0 to 2047;
  signal  v_frontporch      : integer range 0 to 2047;
  signal  v_sync_time       : integer range 0 to 2047;
  signal  v_backporch       : integer range 0 to 2047;
  
  signal active_frame       : std_logic;

begin

-------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------

-- definisanje parametara potrebnih za sihronizacione signale
-- ovi parametri zavise od rezolucije

res_0 : if ( HORIZONTAL_RES = 64 and VERTICAL_RES = 48 ) generate

            h_pixels       <= 64;
            h_frontporch   <= 2 ;
            h_sync_time    <= 2 ;
            h_backporch    <= 2 ;

            v_lines        <= 48;
            v_frontporch   <= 2 ;
            v_sync_time    <= 2 ;
            v_backporch    <= 2 ;

       end generate res_0;

res_1 : if ( HORIZONTAL_RES = 640 and VERTICAL_RES = 480 ) generate

            h_pixels       <= 640;
            h_frontporch   <= 16 ;
            h_sync_time    <= 96 ;
            h_backporch    <= 40 ;

            v_lines        <= 480;
            v_frontporch   <= 11 ;
            v_sync_time    <= 2  ;
            v_backporch    <= 31 ;

       end generate res_1;

res_2 : if ( HORIZONTAL_RES = 800 and VERTICAL_RES = 600 ) generate

            h_pixels       <= 800;
            h_frontporch   <= 56 ;
            h_sync_time    <= 120;
            h_backporch    <= 64 ;

            v_lines        <= 600;
            v_frontporch   <= 37 ;
            v_sync_time    <= 6  ;
            v_backporch    <= 23 ;

       end generate res_2;

res_3 : if ( HORIZONTAL_RES = 1024 and VERTICAL_RES = 768 ) generate

            h_pixels       <= 1024;
            h_frontporch   <= 24  ;
            h_sync_time    <= 136 ;
            h_backporch    <= 144 ;

            v_lines        <= 768 ;
            v_frontporch   <= 3   ;
            v_sync_time    <= 6   ;
            v_backporch    <= 29  ;

       end generate res_3;

res_4 : if ( HORIZONTAL_RES = 1152 and VERTICAL_RES = 864 ) generate

            h_pixels       <= 1152;
            h_frontporch   <= 64  ;
            h_sync_time    <= 128 ;
            h_backporch    <= 256 ;

            v_lines        <= 864 ;
            v_frontporch   <= 1   ;
            v_sync_time    <= 3   ;
            v_backporch    <= 32  ;

       end generate res_4;

res_5 : if ( HORIZONTAL_RES = 1280 and VERTICAL_RES = 1024 ) generate

            h_pixels       <= 1280;
            h_frontporch   <= 48  ;
            h_sync_time    <= 112 ;
            h_backporch    <= 248 ;

            v_lines        <= 1024;
            v_frontporch   <= 1   ;
            v_sync_time    <= 3   ;
            v_backporch    <= 38  ;

       end generate res_5;

-------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------
--
--       |<--- active region --->|<----------- blanking region ---------->|<--- active region --->|<----------- blanking region ---------->|
--       |       (pixels)        |                                        |       (pixels)        |                                        |
--       |       (lines)         |                                        |       (lines)         |                                        |
--       |                       |                                        |                       |                                        |
--   ----+---------- ... --------+-------------             --------------+---------- ... --------+-------------             --------------+--
--   |   |                       |            |             |             |                       |            |             |             |
--   |   |                       |<--front    |<---sync     |<---back     |                       |<--front    |<---sync     |<---back     |
--   |   |                       |    porch-->|     time--->|    porch--->|                       |    porch-->|     time--->|    porch--->|
------   |                       |            ---------------             |                       |            ---------------             |
--       |                       |                                        |                       |                                        |
--       |<------------------- period ----------------------------------->|<------------------- period ----------------------------------->|
--
-------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------



-- broji od nule do pune velicine linije
process(clk_i)
begin

  if ( clk_i'event and clk_i = '1' ) then

      if ( rst_n_i = '0' ) then  h_count_r <= (others => '0');
      else
        if   ( h_count_r < ( h_sync_time + h_pixels + h_frontporch + h_backporch) ) then  h_count_r <= h_count_r + 1;
        else                                                                              h_count_r <= (others => '0');
        end if;
      end if;

  end if;

end process;

-- generise hsyncb : nula je nad hor sync_o
process(clk_i)
begin

 if ( clk_i'event and clk_i = '1') then

   if ( rst_n_i = '0') then horiz_sync_r <= '1';
   else
     if  ( (h_count_r >= (h_frontporch + h_pixels)) and (h_count_r < (h_pixels + h_frontporch + h_sync_time) )) then  horiz_sync_r <= '0';
     else                                                                                                             horiz_sync_r <= '1';
     end if;
   end if;

 end if;

end process;

-- uvecava vcnt na rastucu ivicu hor sync_o
process(clk_i)
begin

 if (clk_i'event and clk_i = '1') then

     if ( rst_n_i = '0' ) then   v_count_r <= (others => '0');
     else
       if ( h_count_r = h_pixels + h_frontporch + h_sync_time ) then
         if ( v_count_r < (v_sync_time + v_lines + v_frontporch + v_backporch) ) then  v_count_r <= v_count_r + 1;
         else                                                                          v_count_r <= (others => '0');
         end if;
       end if;
    end if;

 end if;

end process;


process(clk_i)
begin

 if (clk_i'event and clk_i = '1') then

   if ( rst_n_i = '0' ) then vert_sync_r <= '1';
   else
     if ( h_count_r = h_pixels + h_frontporch + h_sync_time ) then
        if   (v_count_r >= (v_lines + v_frontporch) and v_count_r < (v_lines + v_frontporch + v_sync_time)) then  vert_sync_r <= '0';
        else                                                                                                      vert_sync_r <= '1';
        end if;
     end if;
   end if;

 end if;

end process;


process (h_count_r,v_count_r,h_pixels, v_lines)
begin
        if ( (h_count_r >= h_pixels) or (v_count_r >= v_lines) ) then  enable_s <= '0';
        else                                                           enable_s <= '1';
        end if;

end process;


-------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------



  -- registrovanje signala
reg_outputs_1:process (clk_i)

  begin

    if (clk_i'event and clk_i = '1') then

      if (rst_n_i = '0') then

        horiz_sync_out_r   <= '0';
        vert_sync_out_r    <= '0';
        psave_r            <= '0';
        blank_r            <= '0';
        sync_r             <= '0';

        horiz_sync_out_d_r <= '0';
        vert_sync_out_d_r  <= '0';
        psave_d_r          <= '0';
        blank_d_r          <= '0';
        sync_d_r           <= '0';


      else


         horiz_sync_out_d_r <= horiz_sync_r                ;
         vert_sync_out_d_r  <= vert_sync_r                 ;
         psave_d_r          <= '1'                         ;
         blank_d_r          <= enable_s                    ;
         sync_d_r           <= vert_sync_r and horiz_sync_r;

         horiz_sync_out_r   <=  horiz_sync_out_d_r;
         vert_sync_out_r    <=  vert_sync_out_d_r ;
         psave_r            <=  psave_d_r         ;
         blank_r            <=  blank_d_r         ;
         sync_r             <=  sync_d_r          ;



      end if;

    end if;

end process reg_outputs_1;

  reg_outputs_2:process (clk_i)
  begin
    if (clk_i'event and clk_i = '1') then
      if (rst_n_i = '0') then
        red_r   <= (others => '0');
        green_r <= (others => '0');
        blue_r  <= (others => '0');
      else
        if ( enable_s = '1' ) then
          if (direct_mode_i = '1') then
            red_r   <= dir_red_i;
            green_r <= dir_green_i;
            blue_r  <= dir_blue_i;
          elsif (show_frame_i = '1' and active_frame = '1') then
            red_r   <= frame_color_i(23 downto 16);
            green_r <= frame_color_i(15 downto 8);
            blue_r  <= frame_color_i(7 downto 0);
          elsif (active_pixel_i = '1') then
            red_r   <= foreground_color_i(23 downto 16);
            green_r <= foreground_color_i(15 downto 8);
            blue_r  <= foreground_color_i(7 downto 0);
          else
            red_r   <= background_color_i(23 downto 16);
            green_r <= background_color_i(15 downto 8);
            blue_r  <= background_color_i(7 downto 0);
          end if;
        end if;
      end if;
    end if;
  end process reg_outputs_2; 

  process (v_count_r,h_count_r)begin
    if (--okvir
      v_count_r < FRAME_SIZE                      or
      v_count_r > (VERTICAL_RES - FRAME_SIZE-1)   or
      h_count_r < FRAME_SIZE                      or
      h_count_r > (HORIZONTAL_RES - FRAME_SIZE-1)) then
       active_frame <= '1';
     else
       active_frame <= '0';
     end if;
   end process;

  -- povezivanje signala na izlaz

  red_o          <=  red_r           ;
  green_o        <=  green_r         ;
  blue_o         <=  blue_r          ;
  horiz_sync_o   <=  horiz_sync_out_r;
  vert_sync_o    <=  vert_sync_out_r ;
  pixel_row_o    <=  v_count_r       ;
  pixel_column_o <=  h_count_r       ;
  psave_o        <=  psave_r         ;
  blank_o        <=  blank_r         ;
  pix_clk_o      <=  clk_i           ;
  sync_o         <=  sync_r          ;

end rtl;

