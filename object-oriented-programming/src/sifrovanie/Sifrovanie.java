package sifrovanie;

import javax.swing.*;

/*
 * Rozhranie, ktoré implementujú jednotlivé typy šifrovania
 */
public interface Sifrovanie {
	/*
	 * Vráti zašifrovaný bajt
	 */
	byte zasifruj (byte data) throws ParametreVynimka;
	
	/*
	 * Vráti dešifrovaný bajt
	 */
	byte desifruj (byte data) throws ParametreVynimka;
	
	/*
	 * Pridá svoje špecifické prvky do JPanelu
	 */
	void vytvorGUI(JPanel panel);
}

/*
 * Nesprávne parametre šifrovania, napr. zlý formát kľúča
 */
class ParametreVynimka extends Exception {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	ParametreVynimka(String s)
	{
		super(s);
	}
}