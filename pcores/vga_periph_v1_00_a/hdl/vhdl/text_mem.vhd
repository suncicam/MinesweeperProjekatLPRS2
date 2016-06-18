-------------------------------------------------------------------------------
--  Department of Computer Engineering and Communications
--  Author: LPRS2  <lprs2@rt-rk.com>
--
--  Module Name: text_mem
--
--  Description:
--
--    Dual-port RAM for Text (cahar address in char_rom)
--
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity text_mem is
  generic(
    MEM_ADDR_WIDTH : natural := 32;
    MEM_DATA_WIDTH : natural := 32;
    MEM_SIZE       : natural := 4800
    );
  port(
    clk_i     : in  std_logic;
    reset_n_i : in  std_logic;             
    wr_addr_i : in  std_logic_vector(MEM_ADDR_WIDTH-1 downto 0);     -- Slave address input
    rd_addr_i : in  std_logic_vector(MEM_ADDR_WIDTH-1 downto 0);     -- Slave address input
    wr_data_i : in  std_logic_vector(MEM_DATA_WIDTH-1 downto 0);     -- Write data output
    we_i      : in  std_logic;                                       -- 1-write transaction
    rd_data_o : out std_logic_vector(MEM_DATA_WIDTH-1 downto 0)      -- read data output
    );
end entity;

architecture arc_text_mem of text_mem is

  type t_text_mem  is array (0 to MEM_SIZE-1) of  std_logic_vector(MEM_DATA_WIDTH-1 downto 0);

  signal index_t : natural;
  signal index   : natural;

  signal text_mem : t_text_mem := (
          0 => "001100",
          1 => "010000",
          2 => "010010",
          3 => "010011",
          4 => "100000",
          5 => "110010",
          others => (others => '0')
          );
  
begin
  
  DP_TEXT_MEM : process (clk_i) begin
    if (rising_edge(clk_i)) then
      if (we_i = '1') then
        text_mem(conv_integer(wr_addr_i)) <= wr_data_i; -- update img_mem from out_mem
      end if;
      rd_data_o <= text_mem(conv_integer(index));
    end if;
  end process;
  
  index_t <= conv_integer(rd_addr_i);
  index   <= index_t when (index_t < text_mem'length) else 0;

end arc_text_mem;