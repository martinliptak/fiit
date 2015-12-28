package sifrovanie;

/*
 * Emuluje čítanie znakov z JTextField. Rozširuje TextoveGUI
 */
public class TextovyVstup extends TextoveGUI implements Vstup {
	protected int pozicia;
	
	/*
	 * Pamätá si pozíciu a vráti ďalší znak zo Stringu
	 */
	@Override
	public byte precitaj() throws CitanieVynimka {
		byte[] znaky = text.getText().getBytes();
		 
		if (pozicia < znaky.length)
			return znaky[pozicia++];
		else
			throw new CitanieVynimka("Toto sa nemalo stať");
	}

	/*
	 * Na začiatok
	 */
	@Override
	public void nastavVstup() {
		pozicia = 0;
	}

	/*
	 * Či nie je na konci
	 */
	@Override
	public boolean mozeCitat() {
		byte[] znaky = text.getText().getBytes();
		
		return (pozicia < znaky.length);
	}
	
}
