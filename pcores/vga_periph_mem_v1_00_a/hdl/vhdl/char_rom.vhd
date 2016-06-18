-----------------------------------------------------------------------------------
--  Odsek za racunarsku tehniku i medjuracunarske komunikacije                   --
--  Copyright © 2009 All Rights Reserved                                         --
--                                                                               --
--  Projekat: LabVezba2                                                          --
--  Ime modula: char_rom.vhd                                                     --
--  Autori: LPRS2 TIM 2009/2010 <LPRS2@KRT.neobee.net>                           --
--                                                                               --
--  Opis:                                                                        --
--          Char_rom generise tekst na ekranu.                                   --
--          Znak se predstavlja matricom 8x8 tacaka.                             --
--          Oblici znakova se nalaze u datoteci char_rom_def_mem.coe             --
--                                                                               --
-----------------------------------------------------------------------------------

LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_ARITH.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;



ENTITY char_rom IS PORT (
                         clk_i                : IN  STD_LOGIC;                            -- takt SIGNAL
                         character_address_i  : IN  STD_LOGIC_VECTOR (5 DOWNTO 0);        -- adresa karaktera
                         font_row_i           : IN  STD_LOGIC_VECTOR (2 DOWNTO 0);        -- ispis reda
                         font_col_i           : IN  STD_LOGIC_VECTOR (2 DOWNTO 0);        -- ispis kolone
                         rom_mux_output_o     : OUT STD_LOGIC                             -- izlazni SIGNAL iz char_rom-a
                        );
END char_rom;



ARCHITECTURE Behavioral OF char_rom IS

  SIGNAL rom_data    : STD_LOGIC_VECTOR( 7 DOWNTO 0 );       -- prosledjuje izlaz iz char_rom-a na ulaz u VGA
  SIGNAL rom_address : STD_LOGIC_VECTOR( 8 DOWNTO 0 );       -- preuzima character_address_i i font_row_i


COMPONENT char_rom_def IS PORT (
                                clk  : IN  STD_LOGIC;                         -- takt
                                addr : IN  STD_LOGIC_VECTOR(8 DOWNTO 0);      -- adresa znaka
                                dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)       -- izlaz
                               );
END COMPONENT;


BEGIN

-- Oblici znakova se nalaze u datoteci char_rom_def_mem.coe

BRAM_MEM_I: char_rom_def PORT MAP (
                                   clk  => clk_i      ,
                                   addr => rom_address,
                                   dout => rom_data
                                  );

                  ------------------|----------
                  --   ADDRESS      | OFFSET  |
                  ------------------|----------

rom_address    <= character_address_i & font_row_i;


PROCESS(font_col_i, rom_data) BEGIN

    CASE(font_col_i) IS

        WHEN  "000" => rom_mux_output_o <= rom_data(7);
        WHEN  "001" => rom_mux_output_o <= rom_data(6);
        WHEN  "010" => rom_mux_output_o <= rom_data(5);
        WHEN  "011" => rom_mux_output_o <= rom_data(4);
        WHEN  "100" => rom_mux_output_o <= rom_data(3);
        WHEN  "101" => rom_mux_output_o <= rom_data(2);
        WHEN  "110" => rom_mux_output_o <= rom_data(1);
        WHEN  "111" => rom_mux_output_o <= rom_data(0);
        WHEN OTHERS => rom_mux_output_o <= '0';

    END CASE;

END PROCESS;



END Behavioral;

