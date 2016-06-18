-------------------------------------------------------------------------------
--  Department of Computer Engineering and Communications
--  Author: LPRS2  <lprs2@rt-rk.com>
--
--  Module Name: vga_top
--
--  Description:
--
--    Top of VGA control with graphics and text
--
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity vga_top is 
  generic (
    H_RES                : natural := 640;
    V_RES                : natural := 480;
    MEM_ADDR_WIDTH       : natural := 32;
    GRAPH_MEM_ADDR_WIDTH : natural := 32;
    TEXT_MEM_DATA_WIDTH  : natural := 32;
    GRAPH_MEM_DATA_WIDTH : natural := 32;
    RES_TYPE             : natural := 0;       
    MEM_SIZE             : natural := 4800
    );
  port (
    clk_i               : in  std_logic;
    reset_n_i           : in  std_logic;
    --
    direct_mode_i       : in  std_logic; -- 0 - text and graphics interface mode, 1 - direct mode (direct force RGB component)
    dir_red_i           : in  std_logic_vector(7 downto 0);
    dir_green_i         : in  std_logic_vector(7 downto 0);
    dir_blue_i          : in  std_logic_vector(7 downto 0);
    dir_pixel_column_o  : out std_logic_vector(10 downto 0);
    dir_pixel_row_o     : out std_logic_vector(10 downto 0);
    -- mode interface
    display_mode_i      : in  std_logic_vector(1 downto 0);  -- 01 - text mode, 10 - graphics mode, 11 - text and graphics
    -- text mode interface
    text_addr_i         : in  std_logic_vector(MEM_ADDR_WIDTH-1 downto 0);
    text_data_i         : in  std_logic_vector(TEXT_MEM_DATA_WIDTH-1 downto 0);
    text_we_i           : in  std_logic;
    -- graphics mode interface
    graph_addr_i        : in  std_logic_vector(GRAPH_MEM_ADDR_WIDTH-1 downto 0);
    graph_data_i        : in  std_logic_vector(GRAPH_MEM_DATA_WIDTH-1 downto 0);
    graph_we_i          : in  std_logic;
    -- cfg
    font_size_i         : in  std_logic_vector(3 downto 0);
    show_frame_i        : in  std_logic;
    foreground_color_i  : in  std_logic_vector(23 downto 0);
    background_color_i  : in  std_logic_vector(23 downto 0);
    frame_color_i       : in  std_logic_vector(23 downto 0);
    -- vga
    vga_hsync_o         : out std_logic;
    vga_vsync_o         : out std_logic;
    blank_o             : out std_logic;
    pix_clock_o         : out std_logic;
    vga_rst_n_o         : out std_logic;
    psave_o             : out std_logic;
    sync_o              : out std_logic;
    red_o               : out std_logic_vector(7 downto 0); 
    green_o             : out std_logic_vector(7 downto 0); 
    blue_o              : out std_logic_vector(7 downto 0)
  );
end vga_top;

architecture rtl of vga_top is

  component vga is generic (
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
    -- cfg
    show_frame_i        : in  std_logic;
    active_pixel_i      : in  std_logic;
    foreground_color_i  : in  std_logic_vector(23 downto 0);
    background_color_i  : in  std_logic_vector(23 downto 0);
    frame_color_i       : in  std_logic_vector(23 downto 0);
    -- vga
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
  end component vga;

  component char_rom 
  is port (
    clk_i               : in   std_logic;
    character_address_i : in   std_logic_vector (5 downto 0);
    font_row_i          : in   std_logic_vector (2 downto 0);
    font_col_i          : in   std_logic_vector (2 downto 0);
    rom_mux_output_o    : out  std_logic
  );
  end component char_rom;

  component text_mem
  generic(
    MEM_ADDR_WIDTH : natural := 20;
    MEM_DATA_WIDTH : natural := 32;
    MEM_SIZE       : natural := 4800
    );
  port(
    clk_i     : in  std_logic;
    reset_n_i : in  std_logic;
    --
    wr_addr_i : in  std_logic_vector(MEM_ADDR_WIDTH-1 downto 0);
    rd_addr_i : in  std_logic_vector(MEM_ADDR_WIDTH-1 downto 0);
    wr_data_i : in  std_logic_vector(MEM_DATA_WIDTH-1 downto 0);
    we_i      : in  std_logic;
    rd_data_o : out std_logic_vector(MEM_DATA_WIDTH-1 downto 0) 
    );
  end component;

  component graphics_mem is
  generic(
    MEM_ADDR_WIDTH : natural := 32;
    MEM_DATA_WIDTH : natural := 32;
    MEM_SIZE       : natural := 4800
    );
  port(
    clk_i     : in  std_logic;
    reset_n_i : in  std_logic;
    --          
    wr_addr_i : in  std_logic_vector(MEM_ADDR_WIDTH-1 downto 0);
    rd_addr_i : in  std_logic_vector(MEM_ADDR_WIDTH-1 downto 0);
    wr_data_i : in  std_logic_vector(MEM_DATA_WIDTH-1 downto 0);
    we_i      : in  std_logic;
    rd_data_o : out std_logic
    );
  end component;

  -- rezolucija ekrana
  signal horizontal_res         : std_logic_vector(11-1 downto 0);
  signal horizontal_res_c       : std_logic_vector(11-1 downto 0);
  signal vertical_res           : std_logic_vector(11-1 downto 0);
  
  signal grid_size              : integer; -- size of step based on char size
  
  signal pix_clk_s              : std_logic;
  signal vga_rst_n_s            : std_logic;
  
  signal pixel_row_s            : std_logic_vector(11-1 downto 0);
  signal pixel_column_s         : std_logic_vector(11-1 downto 0);
  signal pixel_row_c            : std_logic_vector(11-1 downto 0);
  signal pixel_column_c         : std_logic_vector(11-1 downto 0);
  
  -- char rom
  signal char_addr_s            : std_logic_vector(TEXT_MEM_DATA_WIDTH-1 downto 0);
  signal font_col_s             : std_logic_vector(2 downto 0);
  signal font_row_s             : std_logic_vector(2 downto 0);
  signal rom_out_s              : std_logic;
  
  -- tx_rom
  signal txt_rom_addr_c         : std_logic_vector(2*11-1 downto 0);
  signal txt_ram_addr_s         : std_logic_vector(MEM_ADDR_WIDTH-1 downto 0);
  
  signal graph_pixel_addr_c     : std_logic_vector(2*11-1 downto 0);
  signal graph_pixel_addr_s     : std_logic_vector(GRAPH_MEM_ADDR_WIDTH-1 downto 0);
  signal graph_pixel_s          : std_logic;
  signal active_pixel_s         : std_logic;

begin

  vga_i:vga
  generic map(
    RESOLUTION_TYPE => RES_TYPE,
    H_RES           => H_RES,
    V_RES           => V_RES
    )
  port map (
    clk_i               => clk_i,
    rst_n_i             => reset_n_i,
    --
    direct_mode_i       => direct_mode_i,
    dir_red_i           => dir_red_i,
    dir_green_i         => dir_green_i,
    dir_blue_i          => dir_blue_i,
    
    -- cfg
    show_frame_i        => show_frame_i,
    active_pixel_i      => active_pixel_s,
    foreground_color_i  => foreground_color_i,
    background_color_i  => background_color_i,
    frame_color_i       => frame_color_i,
    -- vga
    red_o               => red_o,
    green_o             => green_o,
    blue_o              => blue_o,
    pixel_row_o         => pixel_row_s,
    pixel_column_o      => pixel_column_s,
    hsync_o             => vga_hsync_o,
    vsync_o             => vga_vsync_o,
    psave_o             => psave_o,
    blank_o             => blank_o,
    vga_pix_clk_o       => pix_clk_s,
    vga_rst_n_o         => vga_rst_n_s,
    sync_o              => sync_o
  );
  dir_pixel_column_o <= pixel_column_s;
  dir_pixel_row_o <= pixel_row_s;
  -- multiplex source
  active_pixel_s <= rom_out_s                   when (display_mode_i = "01") else 
                    graph_pixel_s               when (display_mode_i = "10") else
                    rom_out_s xor graph_pixel_s when (display_mode_i = "11");

  char_rom_i:char_rom 
  port map(
    clk_i                => pix_clk_s,
    character_address_i  => char_addr_s,        -- char address
    font_row_i           => font_row_s,         -- font size
    font_col_i           => font_col_s,         -- font size
    rom_mux_output_o     => rom_out_s           -- char pixel value
  );

  text_mem_i:text_mem
  generic map(
    MEM_ADDR_WIDTH => MEM_ADDR_WIDTH,
    MEM_DATA_WIDTH => TEXT_MEM_DATA_WIDTH,
    MEM_SIZE       => MEM_SIZE
  )
  port map(
    clk_i  => pix_clk_s,
    reset_n_i => vga_rst_n_s,
    --
    wr_addr_i => text_addr_i,
    wr_data_i => text_data_i,
    we_i      => text_we_i,
    rd_addr_i => txt_ram_addr_s,
    rd_data_o => char_addr_s
  );
  
  graphics_mem_i:graphics_mem
  generic map(
    MEM_ADDR_WIDTH => GRAPH_MEM_ADDR_WIDTH,
    MEM_DATA_WIDTH => GRAPH_MEM_DATA_WIDTH,
    MEM_SIZE       => MEM_SIZE*8*8--full size per pixel (MEM_SIZE is defined per char)
  )
  port map(
    clk_i     => pix_clk_s,
    reset_n_i => vga_rst_n_s,
    --
    wr_addr_i => graph_addr_i,
    wr_data_i => graph_data_i,
    we_i      => graph_we_i,
    rd_addr_i => graph_pixel_addr_s,
    rd_data_o => graph_pixel_s
  );
  graph_pixel_addr_c <= pixel_row_s*horizontal_res + pixel_column_s;
  graph_pixel_addr_s <= graph_pixel_addr_c(GRAPH_MEM_ADDR_WIDTH-1 downto 0);

  --txt_ram_addr_s <= pixel_row_s/grid_size *horizontal_res/grid_size + pixel_column_s/grid_size;
  txt_rom_addr_c <= pixel_row_c*horizontal_res_c + pixel_column_c;
  txt_ram_addr_s <= txt_rom_addr_c(MEM_ADDR_WIDTH-1 downto 0);
  
  -- get pixel row and column range according selected font size
  pixel_row_c <= "000000" & pixel_row_s(pixel_row_s'length-1 downto 6) when (font_size_i = 3) else
                 "00000"  & pixel_row_s(pixel_row_s'length-1 downto 5) when (font_size_i = 2) else
                 "0000"   & pixel_row_s(pixel_row_s'length-1 downto 4) when (font_size_i = 1) else
                 "000"    & pixel_row_s(pixel_row_s'length-1 downto 3);

  pixel_column_c <= "000000" & pixel_column_s(pixel_column_s'length-1 downto 6) when (font_size_i = 3) else
                    "00000"  & pixel_column_s(pixel_column_s'length-1 downto 5) when (font_size_i = 2) else
                    "0000"   & pixel_column_s(pixel_column_s'length-1 downto 4) when (font_size_i = 1) else
                    "000"    & pixel_column_s(pixel_column_s'length-1 downto 3);

  horizontal_res   <= conv_std_logic_vector(H_RES, 11);
  horizontal_res_c <= "000000" & horizontal_res(horizontal_res'length-1 downto 6) when (font_size_i = 3) else
                      "00000"  & horizontal_res(horizontal_res'length-1 downto 5) when (font_size_i = 2) else
                      "0000"   & horizontal_res(horizontal_res'length-1 downto 4) when (font_size_i = 1) else
                      "000"    & horizontal_res(horizontal_res'length-1 downto 3);
  
  font_row_s <= pixel_row_s(5 downto 3) when (font_size_i = 3) else
                pixel_row_s(4 downto 2) when (font_size_i = 2) else
                pixel_row_s(3 downto 1) when (font_size_i = 1) else
                pixel_row_s(2 downto 0);
  
  font_col_s <= pixel_column_s(5 downto 3) when (font_size_i = 3) else
                pixel_column_s(4 downto 2) when (font_size_i = 2) else
                pixel_column_s(3 downto 1) when (font_size_i = 1) else
                pixel_column_s(2 downto 0)-1;-- because of synchronous memory read there is one cycle delay with char_addr_s, so font_col and font_row should be delayed also

  grid_size <= 64 when (font_size_i = 3) else
               32 when (font_size_i = 2) else
               16 when (font_size_i = 1) else
               8;

  pix_clock_o <= pix_clk_s;
  vga_rst_n_o <= vga_rst_n_s;

end rtl;