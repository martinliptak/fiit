package sifrovanie;

/*
 * Emuluje zápis znakov do JTextField. Rozširuje TextoveGUI
 */
public class TextovyVystup extends TextoveGUI implements Vystup {
	@Override
	public void zapis(byte data) {
		text.setText(text.getText() + new String(new byte[] { data }));
	}

	@Override
	public void nastavVystup() {
		text.setText("");
	}
	
}
