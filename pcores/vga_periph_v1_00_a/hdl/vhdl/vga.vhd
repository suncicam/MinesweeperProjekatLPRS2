-------------------------------------------------------------------------------
--  Department of Computer Engineering and Communications                     
--  Author: LPRS2  <lprs2@rt-rk.com>                                          
--                                                                            
--  Module Name: vga                                                     
--                                                                            
--  Description:                                                              
--                                                                            
--    Instantiate DCM and vga_sync with resolution parameter
--                                                                            
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity vga is 
  generic (
    RESOLUTION_TYPE : natural := 0;
    H_RES           : natural := 640;
    V_RES           : natural := 480
 );
  port(
    clk_i               : in  std_logic;
    rst_n_i             : in  std_logic;
    --
    direct_mode_i       : in  std_logic; -- 0 - text and graphics interface mode, 1 - direct mode (direct force RGB component)
    dir_red_i           : in  std_logic_vector(7 downto 0);
    dir_green_i         : in  std_logic_vector(7 downto 0);
    dir_blue_i          : in  std_logic_vector(7 downto 0);
    --
    show_frame_i        : in  std_logic;
    active_pixel_i      : in  std_logic;
    foreground_color_i  : in  std_logic_vector(23 downto 0);
    background_color_i  : in  std_logic_vector(23 downto 0);
    frame_color_i       : in  std_logic_vector(23 downto 0);
    red_o               : out std_logic_vector(7 downto 0);
    green_o             : out std_logic_vector(7 downto 0);
    blue_o              : out std_logic_vector(7 downto 0);
    pixel_row_o         : out std_logic_vector(10 downto 0);
    pixel_column_o      : out std_logic_vector(10 downto 0);
    hsync_o             : out std_logic;
    vsync_o             : out std_logic;
    psave_o             : out std_logic;
    blank_o             : out std_logic;
    vga_pix_clk_o       : out std_logic;
    vga_rst_n_o         : out std_logic;
    sync_o              : out std_logic
  );
end vga;

architecture rtl of vga is

  component vga_sync is generic (
    HORIZONTAL_RES : natural := 800;
    VERTICAL_RES   : natural := 600;
    FRAME_SIZE     : natural := 4
  );
  port (
    clk_i               : in  std_logic;
    rst_n_i             : in  std_logic;
    --
    direct_mode_i       : in  std_logic; -- 0 - text and graphics interface mode, 1 - direct mode (direct force RGB component)
    dir_red_i           : in  std_logic_vector(7 downto 0);
    dir_green_i         : in  std_logic_vector(7 downto 0);
    dir_blue_i          : in  std_logic_vector(7 downto 0);
    --
    show_frame_i        : in  std_logic;
    active_pixel_i      : in  std_logic;
    foreground_color_i  : in  std_logic_vector(23 downto 0);
    background_color_i  : in  std_logic_vector(23 downto 0);
    frame_color_i       : in  std_logic_vector(23 downto 0);
    red_o               : out std_logic_vector(7 downto 0);
    green_o             : out std_logic_vector(7 downto 0);
    blue_o              : out std_logic_vector(7 downto 0);
    horiz_sync_o        : out std_logic;
    vert_sync_o         : out std_logic;
    pixel_row_o         : out std_logic_vector(10 downto 0);
    pixel_column_o      : out std_logic_vector(10 downto 0);
    psave_o             : out std_logic;
    blank_o             : out std_logic;
    pix_clk_o           : out std_logic;
    sync_o              : out std_logic
  );
  end component vga_sync;
  
  component dcm25MHz
  port(
    CLK_IN1           : in     std_logic;
    CLK_OUT1          : out    std_logic;
    RESET             : in     std_logic;
    LOCKED            : out    std_logic
   );
  end component;
  
  component dcm50MHz
  port(
    CLK_IN1           : in     std_logic;
    CLK_OUT1          : out    std_logic;
    RESET             : in     std_logic;
    LOCKED            : out    std_logic
   );
  end component;
  
  component dcm75MHz
  port(
    CLK_IN1           : in     std_logic;
    CLK_OUT1          : out    std_logic;
    RESET             : in     std_logic;
    LOCKED            : out    std_logic
   );
  end component;
  
  component dcm108MHz
  port(
    CLK_IN1           : in     std_logic;
    CLK_OUT1          : out    std_logic;
    RESET             : in     std_logic;
    LOCKED            : out    std_logic
   );
  end component;
  
  component srl16 port (
    a0  : in  std_logic;
    a1  : in  std_logic;
    a2  : in  std_logic;
    a3  : in  std_logic;
    clk : in  std_logic;
    d   : in  std_logic;
    q   : out std_logic);
  end component srl16;

  -- signali
  signal  rst_s               : std_logic;   -- invertovani ulazni reset, povezuje se na dcm, sluzi za resetovanje dcm-a
  signal  clk_s               : std_logic;   -- izlazni takt iz dcm-a
  signal  locked_s            : std_logic;   -- signal locked iz dcm-a
  signal  locked_del_s        : std_logic;   -- zakasnjeni signal locked iz dcm-a
  signal  locked_del_reg_r    : std_logic;   -- registrovan zakasnjeni signal locked iz dcm-a

begin

  rst_s <= NOT rst_n_i;

  SRL16_inst:SRL16 PORT MAP(
    CLK  => clk_s,               -- Clock     input
    D    => locked_s,            -- SRL data  input
    A0   => '1',                 -- Select[0] input
    A1   => '1',                 -- Select[1] input
    A2   => '1',                 -- Select[2] input
    A3   => '1',                 -- Select[3] input
    Q    => locked_del_s         -- SRL data  output
  );

  process (clk_s)
  begin
    if (clk_s'event and clk_s='1') then
      if ( rst_n_i = '0' )  then
        locked_del_reg_r <='0';
      else
        locked_del_reg_r <= locked_del_s;
      end if;
    end if;
  end process;
  vga_rst_n_o <= locked_del_reg_r;

  -- povezivanje sa vga_sync modulom
  vga_sync_i:vga_sync
  generic map(
    HORIZONTAL_RES => H_RES,
    VERTICAL_RES   => V_RES
  )
  port map(
    clk_i              => clk_s,
    rst_n_i            => locked_del_reg_r,
    --
    direct_mode_i      => direct_mode_i,
    dir_red_i          => dir_red_i,
    dir_green_i        => dir_green_i,
    dir_blue_i         => dir_blue_i,
    show_frame_i       => show_frame_i,
    active_pixel_i     => active_pixel_i,
    foreground_color_i => foreground_color_i,
    background_color_i => background_color_i,
    frame_color_i      => frame_color_i,
    red_o              => red_o,
    green_o            => green_o,
    blue_o             => blue_o,
    horiz_sync_o       => hsync_o,
    vert_sync_o        => vsync_o,
    pixel_row_o        => pixel_row_o,
    pixel_column_o     => pixel_column_o,
    psave_o            => psave_o,
    blank_o            => blank_o,
    pix_clk_o          => vga_pix_clk_o,
    sync_o             => sync_o
  );

  -- u zavisnosti od prametra resolution_type se vrsi instanciranje komponenti (DCM)
  res_0: if ( RESOLUTION_TYPE = 0) generate
    dcm25_i : dcm25mhz port map(
      clk_in1   => clk_i,
      clk_out1  => clk_s,
      reset     => rst_s,
      locked    => locked_s
      );
  end generate res_0;

  res_1: if ( RESOLUTION_TYPE = 1 ) generate
    dcm25_i : dcm25mhz port map(
      clk_in1   => clk_i,
      clk_out1  => clk_s,
      reset     => rst_s,
      locked    => locked_s
      );
  end generate res_1;

  res_2: if ( RESOLUTION_TYPE = 2 ) generate
    dcm50_i : dcm50mhz port map(
      clk_in1   => clk_i,
      clk_out1  => clk_s,
      reset     => rst_s,
      locked    => locked_s
    );
  end generate res_2;

  res_3: if ( RESOLUTION_TYPE = 3 ) generate
    dcm75_i : dcm75mhz port map(
      clk_in1   => clk_i,
      clk_out1  => clk_s,
      reset     => rst_s,
      locked    => locked_s
      );
  end generate res_3;

  res_4: if ( RESOLUTION_TYPE = 4 ) generate
    dcm108_i : dcm108mhz port map(
      clk_in1   => clk_i,
      clk_out1  => clk_s,
      reset     => rst_s,
      locked    => locked_s
    );
  end generate res_4;

  res_5: if ( RESOLUTION_TYPE = 5 ) generate
    dcm108_i : dcm108mhz port map(
      clk_in1   => clk_i,
      clk_out1  => clk_s,
      reset     => rst_s,
      locked    => locked_s
      );
  end generate res_5;

END rtl;