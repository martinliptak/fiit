package sifrovanie;

import javax.swing.*;

/*
 * Spolocne GUI pre vstup aj vystup
 */
public class TextoveGUI {
	protected JTextArea text;
	
	public void vytvorGUI(JPanel panel) {
		text = new JTextArea(5, 50);
		
		panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));
    	panel.add(new JLabel("Zadajte správu"));
    	panel.add(text);
	}
}
