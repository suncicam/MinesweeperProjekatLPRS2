-------------------------------------------------------------------------------
--  Department of Computer Engineering and Communications
--  Author: LPRS2  <lprs2@rt-rk.com>
--
--  Module Name: graphics_mem
--
--  Description:
--
--    Dual-port RAM for graphics
--
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity graphics_mem is
  generic(
    MEM_ADDR_WIDTH : natural := 32;
    MEM_DATA_WIDTH : natural := 32;
    MEM_SIZE       : natural := 4800
    );
  port(
    clk_i     : in  std_logic;
    reset_n_i : in  std_logic;     
    wr_addr_i : in  std_logic_vector(MEM_ADDR_WIDTH-1 downto 0);     -- write address input
    rd_addr_i : in  std_logic_vector(MEM_ADDR_WIDTH-1 downto 0);     -- read address input
    wr_data_i : in  std_logic_vector(MEM_DATA_WIDTH-1 downto 0);     -- Write data output
    we_i      : in  std_logic;                                       -- 1 - write transaction
    rd_data_o : out std_logic                                        -- read data output
    );
end entity;

architecture arc_graphics_mem of graphics_mem is

  type t_graphics_mem  is array (0 to MEM_SIZE/MEM_DATA_WIDTH-1) of  std_logic_vector(MEM_DATA_WIDTH-1 downto 0);

  signal graphics_mem : t_graphics_mem := (
--        0 => "000000",
--        1 => "000001",
--        2 => "000010",
        others => (others => '0')
        );
  signal mem_up_addr : std_logic_vector(MEM_ADDR_WIDTH-1 downto 0);
  signal mem_lo_addr : std_logic_vector(5-1 downto 0);
  
  signal rd_value : std_logic_vector(MEM_DATA_WIDTH-1 downto 0);
  
  signal index_0_t : natural;
  signal index_0   : natural;
  signal index_1_t : natural;
  signal index_1   : natural;
  signal index_2_t : natural;
  signal index_2   : natural;
  
begin

  -- get address for graphics mem based on memory format
  mem_up_addr <= "000"   & rd_addr_i(MEM_ADDR_WIDTH-1 downto 3) when (MEM_DATA_WIDTH = 8) else
                 "0000"  & rd_addr_i(MEM_ADDR_WIDTH-1 downto 4) when (MEM_DATA_WIDTH = 16) else
                 "00000" & rd_addr_i(MEM_ADDR_WIDTH-1 downto 5);

  mem_lo_addr <= "00" & rd_addr_i(3-1 downto 0) when (MEM_DATA_WIDTH = 8) else
                 '0'  & rd_addr_i(4-1 downto 0) when (MEM_DATA_WIDTH = 16) else
                      rd_addr_i(5-1 downto 0);
  
  DP_GRAPHICS_MEM : process (clk_i) begin
    if (rising_edge(clk_i)) then
      if (we_i = '1') then
        graphics_mem(index_2) <= wr_data_i;
      end if;
      --rd_value <= graphics_mem(conv_integer(index_0));
    end if;
  end process;
  
  DP_GRAPHICS_MEM_RD : process (clk_i) begin
    if (rising_edge(clk_i)) then
      rd_value <= graphics_mem(conv_integer(index_0));
    end if;
  end process;
  rd_data_o <= rd_value(conv_integer(index_1));
  
  index_0_t <= conv_integer(mem_up_addr);
  index_0   <= index_0_t when (index_0_t < graphics_mem'length) else 0;
  
  index_1_t <= conv_integer(mem_lo_addr);
  index_1   <= index_1_t when (index_1_t < graphics_mem'length) else 0;
  
  index_2_t <= conv_integer(wr_addr_i);
  index_2   <= index_2_t when (index_2_t < graphics_mem'length) else 0;

end arc_graphics_mem;