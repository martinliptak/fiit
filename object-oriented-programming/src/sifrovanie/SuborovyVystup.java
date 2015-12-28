package sifrovanie;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;

/*
 * Pre vystup do suboru, metody funguju analogicky ako pri vstupe
 */
public class SuborovyVystup implements Vystup {
	protected FileOutputStream fos; 
	protected File file;
	
	public void vytvorGUI(final JPanel panel) {
		final JButton button = new JButton("Vybrať súbor");
		final JLabel meno = new JLabel();
    	final JFileChooser fc = new JFileChooser();
		
		panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));
    	panel.add(new JLabel("Vyberte súbor"));
    	panel.add(meno);
    	panel.add(button);
    	
    	button.addActionListener(new ActionListener() {
			
			public void actionPerformed(ActionEvent arg0) {
				int returnVal = fc.showSaveDialog(panel);

		        if (returnVal == JFileChooser.APPROVE_OPTION) {
		            file = fc.getSelectedFile();
		            meno.setText(file.getName());
		        }
			}
		});
	}

	@Override
	public void nastavVystup() throws ZapisVynimka {
		try {
			fos = new FileOutputStream(file);
		} catch (FileNotFoundException e) {
			throw new ZapisVynimka("Súbor nebol nájdený");
		}
		
	}

	@Override
	public void zapis(byte data) throws ZapisVynimka {
		try {
			fos.write(data);
		} catch (IOException e) {
			throw new ZapisVynimka(e.getMessage());
		}	
	}
	
}
