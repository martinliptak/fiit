package sifrovanie;

/*
 * Implementuje vstup pre prihlasovacie udaje. Dedi od PrihlasovacieGUI, 
 * lebo to ma spolocne aj s vystupom
 */
public class PrihlasovacieVstup extends PrihlasovacieGUI implements Vstup {
	protected int pozicia;
	
	protected byte[] serializuj () {
		return ("prihlasovanie\t" + domena.getText() + "\t" + meno.getText() + "\t" + heslo.getText() + "\n").getBytes();
	}
	
	/*
	 * Dáta z 3 textfieldov najskôr "serializuje" do Stringu, 
	 * potom vráti další znak podla uloženej pozície
	 */
	@Override
	public byte precitaj() throws CitanieVynimka {
		byte[] znaky = serializuj();
		
		if (pozicia < znaky.length)
			return znaky[pozicia++];
		else
			throw new CitanieVynimka("Toto sa nemalo stať");
	}

	@Override
	public void nastavVstup() {
		pozicia = 0;
	}

	@Override
	public boolean mozeCitat() {
		byte[] znaky = serializuj();
		
		return (pozicia < znaky.length);
	}
	
}
