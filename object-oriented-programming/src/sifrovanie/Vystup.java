package sifrovanie;

import javax.swing.*;

/*
 * Analogicke k vstupu (rozhranie Vstup)
 */
public interface Vystup {
	void zapis(byte data) throws ZapisVynimka;
	void nastavVystup() throws ZapisVynimka;

	void vytvorGUI(JPanel panel); 
}

class ZapisVynimka extends Exception {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	
	ZapisVynimka(String s)
	{
		super(s);
	}
}