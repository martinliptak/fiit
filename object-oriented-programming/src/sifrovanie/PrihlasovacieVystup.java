package sifrovanie;

import java.util.regex.Pattern;

/*
 * Implementuje vystup pre prihlasovacie udaje. Dedi od PrihlasovacieGUI, 
 * lebo to ma spolocne so vstupom
 */
public class PrihlasovacieVystup extends PrihlasovacieGUI implements Vystup {
	protected String cele;
	
	/*
	 * Dáta "deserializuje" a zapíše do jednotlivý textfieldov.
	 * Ošetrí, ak niekto pošle na výstup dáta v zlom formáte.
	 */
	@Override
	public void zapis(byte data) throws ZapisVynimka {
		cele = cele + new String(new byte [] {data});
		if (data == '\n')
		{
			Pattern vzor = Pattern.compile("\t");
			String[] s = vzor.split(cele);
			
			if (! s[0].equals("prihlasovanie"))
				throw new ZapisVynimka("Nespravny format udajov.");
			
			domena.setText(s[1]);
			meno.setText(s[2]);
			heslo.setText(s[3]);
		}
	}

	@Override
	public void nastavVystup() {
		cele = "";
	}
	
}
