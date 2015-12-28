package sifrovanie;

import javax.swing.*;

/*
 * Zašifruje bajty metodou XOR
 */
public class XORSifrovanie implements Sifrovanie {
	private JTextField kluc;
	
	@Override
	public byte desifruj(byte data) throws ParametreVynimka {
		try {
			return (byte) (data ^ Byte.parseByte(kluc.getText()));
		}
		catch (NumberFormatException e) {
			throw new ParametreVynimka("Zadany kluc musi byt cislo mensie ako 128");
		}
	}

	@Override
	public byte zasifruj(byte data) throws ParametreVynimka {
		try {
			return (byte) (data ^ Byte.parseByte(kluc.getText()));
		}
		catch (NumberFormatException e) {
			throw new ParametreVynimka("Zadany kluc musi byt cislo mensie ako 128");
		}
	}

	@Override
	public void vytvorGUI(JPanel panel) {
		kluc = new JTextField();
		
		panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));
    	panel.add(new JLabel("Zadajte kľúč"));
    	panel.add(kluc);
	}

}
