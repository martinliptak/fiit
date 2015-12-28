package sifrovanie;

import javax.swing.*;

/*
 * Rozhranie pre vstup dát
 */
public interface Vstup {
	/*
	 * Prečíta bajt
	 */
	byte precitaj() throws CitanieVynimka;
	
	/*
	 * Nastaví na začiatok/otvorí subor
	 */
	void nastavVstup() throws CitanieVynimka;
	
	/*
	 * Ci nebude koniec suboru/správy
	 */
	boolean mozeCitat();
	
	/*
	 * Pridá svoje prvky do JPanelu
	 */
	void vytvorGUI(JPanel panel); 
}

/*
 * Subor nenájdený, IOException, ... kryte vlatnou vynimkou
 */
class CitanieVynimka extends Exception {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	
	CitanieVynimka(String s)
	{
		super(s);
	}
}
