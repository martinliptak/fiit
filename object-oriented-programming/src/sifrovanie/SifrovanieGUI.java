package sifrovanie;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;

/* 
 * Hlavná trieda programu
 */
public class SifrovanieGUI extends JFrame implements ActionListener {

	private static final long serialVersionUID = 1773289398222810772L;
	
	public static void main(String[] args)
	{
		// Nastaví vzhľad na natívny
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		} catch (InstantiationException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (UnsupportedLookAndFeelException e) {
			e.printStackTrace();
		}
		
		// Vytvorenie a zobrazenie hlavného okna
		SifrovanieGUI frame = new SifrovanieGUI();  
		frame.setVisible(true);
	}
	
	/*
	 * Možností do combo boxov
	 */
	String[] dataMoznosti = {
			"Žiadne", 
			"Správa", 
			"Súbor", 
			"Prihlasovacie údaje"
			};
	String[] sifrovanieMoznosti = {
			"Žiadne", 
			"Posuvné", 
			"XOR"
			};
	
	/*
	 * Kontajnery, ktorých obsah sa zmení v závislosti na type šifrovania/dát
	 */
	JPanel sifrovaniePanel;
	JPanel vstupPanel;
	JPanel vystupPanel;
	
	/*
	 * Referencie na objekty tried jadra
	 */
	Sifrovanie sifrovanie;
	Vstup vstup;
	Vystup vystup;

	/*
	 * Konštruktor
	 */
	public SifrovanieGUI()
	{
		JLabel label;
		
		/*
		 * Použil  som BoxLayout, hral som sa aj s inými, ale s týmto sa mi darilo najviac. 
		 */
		setTitle("Šifrovanie");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setSize(200, 500);
		setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
		
		/*
		 * Label, Combo Box a prázdny Panel pre vstup
		 */
		label = new JLabel("Typ vstupu");
		add(label);
		
		// Použije vyššie vytvorené pole Stringov
		JComboBox vstupBox = new JComboBox(dataMoznosti); 
		// Táto trieda slúži aj ako ActionListener, keby som použil vnorenú triedu, atribúty, 
		// ktoré potrebujem meniť by som musel deklarovať ako final
		vstupBox.addActionListener(this); 
		// Pre ActionListener
		vstupBox.setName("vstup"); 
		// Kvôli BoxLayout
		vstupBox.setMaximumSize(vstupBox.getPreferredSize()); 
		add(vstupBox);
		
		vstupPanel = new JPanel();
		add(vstupPanel);
		
		/*
		 * Analogicky pre šifrovania
		 */
		label = new JLabel("Typ šifrovania");
		add(label);
		
		JComboBox sifrovanieBox = new JComboBox(sifrovanieMoznosti);
		sifrovanieBox.addActionListener(this);
		sifrovanieBox.setName("sifrovanie");
		sifrovanieBox.setMaximumSize(sifrovanieBox.getPreferredSize());
		add(sifrovanieBox);
		
		sifrovaniePanel = new JPanel();
		add(sifrovaniePanel);
		
		/*
		 * Analogicky pre výstup
		 */
		label = new JLabel("Typ vystupu");
		add(label);
		
		JComboBox vystupBox = new JComboBox(dataMoznosti);
		vystupBox.addActionListener(this);
		vystupBox.setName("vystup");
		vystupBox.setMaximumSize(vystupBox.getPreferredSize());
		add(vystupBox);
		
		vystupPanel = new JPanel();
		add(vystupPanel);
		
		/*
		 * Tlačidlá na zašifrovanie alebo dešifrovanie
		 */
		Box box = new Box(BoxLayout.LINE_AXIS);
		JButton zasifruj = new JButton("Zašifruj");
		// Táto trieda aj ako ActionListener
		zasifruj.addActionListener(this);
		// Pre ActionListener
		zasifruj.setName("zasifruj");
		JButton desifruj = new JButton("Dešifruj");
		desifruj.addActionListener(this);
		desifruj.setName("desifruj");
		box.add(desifruj);
		box.add(zasifruj);
		add(box);
	}
	
	/*
	 * ActionListener pre (skoro) všetky prvky GUI
	 */
	public void actionPerformed(ActionEvent e) {
		// Nie pekné, ale najjednoduchšie. Pri stlačení tlačidla na spustenie akcie
		if (e.getSource() instanceof JButton)
		{
			JButton button = (JButton) e.getSource();
			
			if (button.getName().equals("zasifruj") || button.getName().equals("desifruj")) {
				// Pustí hlavnu činnosť programu v samostatnej niti, aby sa GUI nazaseklo 
				// pri väčšom subore
				new Thread(new SifrovanieWorker(this, button.getName())).start();
			}
		}
		// Pri vybratí položky v ComboBoxe
		else if (e.getSource() instanceof JComboBox)
		{
			JComboBox box = (JComboBox) e.getSource();
			int selected = box.getSelectedIndex();

			// Podľa toho, či sa vyberá vstup, sifrovanie alebo vystup
			if (box.getName().equals("vstup")) {
				// vstupPanel je hneď pod vstupCombo
				// Vyčistí ho
				vstupPanel.removeAll();
				// a podľa vybratej položky
				switch (selected) {
				case 1:
					// priradi referencii vstup novy objekt
					vstup = new TextovyVstup();
					// a zavola metodu vytvorGUI, ktora v panely vytvori svoje prvky
					vstup.vytvorGUI(vstupPanel);
					break;
				case 2:
					vstup = new SuborovyVstup();
					vstup.vytvorGUI(vstupPanel);
					break;
				case 3:
					vstup = new PrihlasovacieVstup();
					vstup.vytvorGUI(vstupPanel);
					break;
				default:
					vstup = null;
				}
				vstupPanel.setMaximumSize(vstupPanel.getPreferredSize());
				// Aby sa zmeny prejavili
				vstupPanel.revalidate();
				vstupPanel.repaint();
			}
			else if (box.getName().equals("sifrovanie")) {
				// Analogické ako pre vstup...
				sifrovaniePanel.removeAll();
				switch (selected) {
				case 1:
					sifrovanie = new PosuvneSifrovanie();
					sifrovanie.vytvorGUI(sifrovaniePanel);
					break;
				case 2:
					sifrovanie = new XORSifrovanie();
					sifrovanie.vytvorGUI(sifrovaniePanel);
					break;
				default:
					sifrovanie = null;
				}
				sifrovaniePanel.setMaximumSize(sifrovaniePanel.getPreferredSize());
				sifrovaniePanel.revalidate();
				sifrovaniePanel.repaint();
			}
			else if (box.getName().equals("vystup")) {
				vystupPanel.removeAll();
				switch (selected) {
				case 1:
					vystup = new TextovyVystup();
					vystup.vytvorGUI(vystupPanel);
					break;
				case 2:
					vystup = new SuborovyVystup();
					vystup.vytvorGUI(vystupPanel);
					break;
				case 3:
					vystup = new PrihlasovacieVystup();
					vystup.vytvorGUI(vystupPanel);
					break;
				default:
					vystup = null;
				}
				vystupPanel.setMaximumSize(vystupPanel.getPreferredSize());
				vystupPanel.revalidate();
				vystupPanel.repaint();
			}
		}
	}
}

class SifrovanieWorker implements Runnable {
	SifrovanieGUI gui;
	String akcia;

	/*
	 * Cez konštrukto predá hlavný GUI objekt a pristupuje k protected atributom, 
	 * takto sa dal aj ActionListener riešiť
	 */
	SifrovanieWorker(SifrovanieGUI g, String a) {
		gui = g;
		akcia = a;
	}
	
	protected void log(String s)
	{
		System.out.println(s);
	}
	
	/*
	 * Tu vykoná hlavnu činnosť programu
	 */
	@Override
	public void run() {
		log("Začiatok pracovnej nite...");
		log("Používam vstup " + gui.vstup.getClass().getName());
		log("Používam výstup " + gui.vystup.getClass().getName());
		
		if (gui.sifrovanie != null)
			log("Používam šifrovanie " + gui.sifrovanie.getClass().getName());
		
		if (gui.vstup == null || gui.vystup == null)
		{
			// Tieto veci sú povinné
			log("Nebol vybraty vstup alebo vystup");
			
			JOptionPane.showMessageDialog(gui, "Prosim vyberte vstup a vystup", "Chyba", 
					JOptionPane.WARNING_MESSAGE);
		}
		else {
			try {
				// Vykoná inicializáciu príslušného vstpu a výstupu
				log("Nastavenie vstupu");
				gui.vstup.nastavVstup();
				
				log("Nastavenie vystupu");
				gui.vystup.nastavVystup();
			}
			catch (CitanieVynimka v) {
				// Napriklad sa nepodarilo otvorit subor
				log("Chyba pri citani zo vstupu: " + v.getMessage());
				
				JOptionPane.showMessageDialog(gui, "Chyba pri citani zo vstupu: " + v.getMessage(), 
						"Chyba", JOptionPane.ERROR_MESSAGE);
			}
			catch (ZapisVynimka v) {
				// Alebo sa ho nepodarilo otvorit pre zapis
				log("Chyba pri zapise na vystup: " + v.getMessage());
				
				JOptionPane.showMessageDialog(gui, "Chyba pri zapise na vystup:  " + v.getMessage(), 
						"Chyba", JOptionPane.ERROR_MESSAGE);
			}
				
			byte b;
			
			try {
				log("Začiatok hlavnej slučny");
				log("Používam akciu " + akcia);
				// Kym moze citat (aby nehodilo vynimku CitanieVynimka)
				while (gui.vstup.mozeCitat())
				{
					// Precita z prislusneho vstupu (subor, sprava) jeden bajt
					b = gui.vstup.precitaj();
					
					// Ak je nastavene sifrovanie (ak nie je, vstup len priamo posle na vystup)
					if (gui.sifrovanie != null)
						// Podla tlacidla sa rozhodne a zavola metodu prislusneho sifrovania
						if (akcia.equals("desifruj"))
							b = gui.sifrovanie.desifruj(b);
						else if (akcia.equals("zasifruj"))
							b = gui.sifrovanie.zasifruj(b);
			
					// Zapise zasifrovany bajt na vystup
					gui.vystup.zapis(b);
				}
				
				log("Koniec hlavnej slučny");
			}
			catch (CitanieVynimka v) {
				// Pri IOException napr.
				log("Chyba pri citani zo vstupu: " + v.getMessage());
				JOptionPane.showMessageDialog(gui, "Chyba pri citani zo vstupu: " + v.getMessage(), 
						"Chyba", JOptionPane.ERROR_MESSAGE);
			}
			catch (ZapisVynimka v) {
				// Pri IOException napr.
				log("Chyba pri zapise na vystup: " + v.getMessage());
				JOptionPane.showMessageDialog(gui, "Chyba pri zapise do vstupu: " + v.getMessage(), 
						"Chyba", JOptionPane.ERROR_MESSAGE);
			}
			catch (ParametreVynimka v) {
				// Nespravny format zadaneho kluca
				log("Nespravne parametre pre sifrovanie: " + v.getMessage());
				JOptionPane.showMessageDialog(gui, "Nespravne parametre pre sifrovanie: " + v.getMessage(), 
						"Chyba", JOptionPane.ERROR_MESSAGE);
			}
			
			log("Koniec pracovnej nite");
		}
	}
	
	
}
