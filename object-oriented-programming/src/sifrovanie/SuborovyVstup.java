package sifrovanie;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import javax.swing.*;

/*
 * Vstup zo súboru
 */
public class SuborovyVstup implements Vstup {
	protected File file;
	protected FileInputStream fis;
	
	@Override
	public byte precitaj() throws CitanieVynimka {
		try {
			return (byte) fis.read();
		}
		catch (IOException e)
		{
			throw new CitanieVynimka(e.getMessage());
		}
	}

	/*
	 * Suborovy vstup a vystup maju GUI rozdielne (showOpenDialog/showSaveDialog), 
	 * preto nededia zo spoločnej triedy a každá si vytvorGUI prekonáva po svojom
	 */
	@Override
	public void vytvorGUI(final JPanel panel) {
		// final kvoli vnorenom ActionListenery
		final JButton button = new JButton("Vybrať súbor");
		final JLabel meno = new JLabel();
    	final JFileChooser fc = new JFileChooser();
		
		panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));
    	panel.add(new JLabel("Vyberte súbor"));
    	panel.add(meno);
    	panel.add(button);
    	
    	button.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent arg0) {
				int returnVal = fc.showOpenDialog(panel);

		        if (returnVal == JFileChooser.APPROVE_OPTION) {
		        	// Nastavi atribut file na vrateny objekt, 
		        	// pri nastavVstup sa subor otvori
		            file = fc.getSelectedFile();
		            meno.setText(file.getName());
		        }
			}
		});
	}

	/*
	 * Otvorí súbor pre čítanie
	 */
	@Override
	public void nastavVstup() throws CitanieVynimka {
		try {
			fis = new FileInputStream(file);
		}
		catch (FileNotFoundException e) {
			throw new CitanieVynimka("Subor sa nenasiel");
		}
	}

	/*
	 * Zistí, či nie je EOF
	 */
	@Override
	public boolean mozeCitat() {
		try {
			return fis.available() > 0;
		}
		catch (IOException e) {
			return false;
		}
	}
	
}
