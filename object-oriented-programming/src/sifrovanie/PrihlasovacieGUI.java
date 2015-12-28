package sifrovanie;

import javax.swing.*;

/*
 * Spolocne GUI pre vstup aj vystup
 */
public class PrihlasovacieGUI {
	protected JTextField domena;
	protected JTextField meno;
	protected JTextField heslo;
	
	public void vytvorGUI(JPanel panel) {
		meno = new JTextField();
		heslo = new JTextField();
		domena = new JTextField();
		
		panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));
    	panel.add(new JLabel("Domena: "));
    	panel.add(domena);
    	panel.add(new JLabel("Meno: "));
    	panel.add(meno);
    	panel.add(new JLabel("Heslo: "));
    	panel.add(heslo);
	}
}
