/*
 * Created by SharpDevelop.
 * User: Administrator
 * Date: 22/11/2004
 * Time: 17:30
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace MelkorNETGUI
{
	/// <summary>
	/// Description of MainForm.	
	/// </summary>
	public class MainForm : System.Windows.Forms.Form
	{
		private System.ComponentModel.IContainer components;
		private System.Windows.Forms.MenuItem searchSettingsMenuItem;
		private System.Windows.Forms.TextBox sgfStepTextBox;
		private System.Windows.Forms.TextBox nnScoreTextBox;
		private System.Windows.Forms.Button forwardButton;
		private System.Windows.Forms.MenuItem newPlayerMenuItem;
		private System.Windows.Forms.MenuItem exitMenuItem;
		private System.Windows.Forms.MenuItem playerMenu;
		private System.Windows.Forms.Button passButton;
		private System.Windows.Forms.Button forwardToEndButton;
		private System.Windows.Forms.Button scoreButton;
		private System.Windows.Forms.MenuItem menuItem6;
		private System.Windows.Forms.Button forwardStepButton;
		private System.Windows.Forms.MainMenu mainMenu1;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.TextBox whiteByoyomiTextBox;
		private System.Windows.Forms.Button runGTPCommandButton;
		private System.Windows.Forms.MenuItem loadNeuralNetworkMenuItem;
		private System.Windows.Forms.MenuItem newMenuItem;
		private System.Windows.Forms.Label capturedLabel;
		private System.Windows.Forms.Button backButton;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label blackLabel;
		private System.Windows.Forms.TextBox blackCapturedTextBox;
		private System.Windows.Forms.Button backStepButton;
		private System.Windows.Forms.MenuItem logWindowMenuItem;
		private System.Windows.Forms.MenuItem fileMenu;
		private System.Windows.Forms.MenuItem helpMenu;
		private System.Windows.Forms.Label timeLabel;
		private System.Windows.Forms.ComboBox gtpCommandsComboBox;
		private System.Windows.Forms.TextBox whiteCapturedTextBox;
		private System.Windows.Forms.Panel panel3;
		private System.Windows.Forms.TextBox resultTextBox;
		private System.Windows.Forms.Label gameInformationLabel;
		private System.Windows.Forms.MenuItem viewMenu;
		private System.Windows.Forms.MenuItem customiseMenuItem;
		private System.Windows.Forms.Panel panel4;
		private System.Windows.Forms.MenuItem aboutMenuItem;
		private System.Windows.Forms.TextBox gtpArgumentsTextBox;
		private System.Windows.Forms.MenuItem changePlayerMenuItem;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox whiteTimeTextBox;
		private System.Windows.Forms.Button undoButton;
		private System.Windows.Forms.Button clearScoredMovesButton;
		private System.Windows.Forms.Button backToStartButton;
		public System.Windows.Forms.Timer oneSecondTimer;
		private System.Windows.Forms.MenuItem openMenuItem;
		private System.Windows.Forms.TextBox blackByoyomiTextBox;
		private System.Windows.Forms.Label gameControlLabel;
		private System.Windows.Forms.PictureBox boardPictureBox;
		private System.Windows.Forms.Label gtpLabel;
		private System.Windows.Forms.MenuItem saveMenuItem;
		private System.Windows.Forms.MenuItem optionsMenu;
		private System.Windows.Forms.Button scoreBlackMovesButton;
		private System.Windows.Forms.TextBox blackTimeTextBox;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Label whiteLabel;
		private System.Windows.Forms.Button scoreWhiteMovesButton;
		private System.Windows.Forms.MenuItem saveAsMenuItem;
		#region Windows Forms variables
		
		
		#endregion

		private NewDialog newDialog = new NewDialog();
		private SearchSettingsDialog searchSettings;    // This relies on DLL settings so initialise
		                                                // in MainForm constructor
		private LogWindow logWindow;
		
		// State variables
		private string computerName = "";
		private int boardSize = 19;
		private int[] board;
		private bool drawNNScores = false;
		private int nextToPlay, blackPlayer, whitePlayer;
		private const int COMPUTER = 1;
		private const int HUMAN = 2;
		private bool lastMovePass = false;
		private string resultString = "";
		private string SGFFilename = "";
		private bool scoreButtonClicked = false;
		private bool mouseDownOnBoard = false;

		// Board status arrays
		private float[] nnScores;
		private bool[] drawFadedStones;
		private bool[] blackTerritory;
		private bool[] whiteTerritory;
		
		// Mode variables
		public enum Mode {None, OpeningSGF, NavigatingSGF, GameInProgress};
		private Mode mode = Mode.None;
		
		// Time settings - in seconds
		private int initialByoyomiTime = 10*60;
		private int initialByoyomiStones = 25;
		private int currentMainTimeBlack = 20*60;
		private int currentMainTimeWhite = 20*60;
		private int currentByoyomiTimeBlack;
		private int currentByoyomiTimeWhite;
		private int currentByoyomiStonesBlack;
		private int currentByoyomiStonesWhite;
		private bool timedGame = false;
		
		private Thread computerMoveGeneratingThread;
		private bool computerGeneratingMove = false;
		
		private bool highlightLastMove = true;
		private int lastX = -1;
		private int lastY = -1;
		
		private Bitmap boardBackground;
		private int boardImageWidth;
		private int boardImageHeight;
		private Bitmap largeBlackStone;
		private Bitmap largeWhiteStone;
		private float border;
		private float gridStep;
		
		// Useful extern functions
		[DllImport("kernel32.dll")]
		public static extern bool Beep(int freq, int duration);    // Frequency in Hz, duration in msec
		
		public MainForm()
		{
			// The InitializeComponent() call is required for Windows Forms designer support.
			InitializeComponent();
			
			logWindow = new LogWindow(new System.ComponentModel.CancelEventHandler(this.logWindowClosing));
			
			// Initialise the MelkorDLL.dll _before_ any function calls!!!
			byte[] errorString = new byte[2000];
			if (MelkorWrapper.initDll(errorString, 2000) == 0) {
				MessageBox.Show(System.Text.Encoding.ASCII.GetString(errorString));
				MelkorWrapper.cleanupDll();
				// Do Application.Exit() here rather than Close() since
				// we intercept Closing events to check if the user wants to quit
				// This is not necessary here, since we have an error at initialisation
				Application.Exit();
			}
			
			// Start logging DLL output to a file and turn on verbose output
			MelkorWrapper.DllSetVerbose(true);
			MelkorWrapper.DllLogStart("dllOutput.log");

			// Get the computer program name string
			byte[] versionString = new byte[2000];
			MelkorWrapper.DllGetVersionString(versionString, 2000);
			StringBuilder tempString = new StringBuilder();
			tempString.Append("Melkor ");
			tempString.Append(System.Text.Encoding.ASCII.GetString(versionString));
			computerName = tempString.ToString();
			
 			boardBackground = new Bitmap("emptyBoard.jpg");
			largeBlackStone = new Bitmap("largeBlackStone.bmp");
			largeBlackStone.MakeTransparent(Color.FromArgb(224, 149, 47));
			largeWhiteStone = new Bitmap("largeWhiteStone.bmp");
			largeWhiteStone.MakeTransparent(Color.FromArgb(224, 149, 47));
			
			ResetBoard(boardSize);
			
			// Update our copy of the board
			MelkorWrapper.DllGetBoard(board, boardSize);
			
			// Test all the functions
			//int x = -1;
			//int y = -1;
			//MelkorWrapper.DllGenerateMove(ref x, ref y, MelkorWrapper.BLACK);
			//MelkorWrapper.DllGetScoredPoints(nnScores, boardSize, MelkorWrapper.WHITE);
			//MelkorWrapper.DllPlayMove(9, 9, MelkorWrapper.BLACK);

			changeTurn(MelkorWrapper.BLACK);
			blackPlayer = HUMAN;
			whitePlayer = COMPUTER;

			// Set some computer move generating defaults
			MelkorWrapper.DllSetUseDynamicSearchDepth(true);
			MelkorWrapper.DllSetStatusDepth(3);
			MelkorWrapper.DllSetTimePerMove(10);
			MelkorWrapper.DllSetUseGlobalMoveFunction(true);
			MelkorWrapper.DllSetKomi(5.5f);
			MelkorWrapper.DllSetHandicap(0);
			
			// Initialise the searchSettings dialog - it relies on DLL so do it 
			// after the DLL init call, and after we've set the defaults above...
			searchSettings = new SearchSettingsDialog();
 
			currentByoyomiTimeBlack = initialByoyomiTime;
			currentByoyomiTimeWhite = initialByoyomiTime;
			currentByoyomiStonesBlack = initialByoyomiStones;
			currentByoyomiStonesWhite = initialByoyomiStones;
			updateGUITimeDisplay();

			// Reset the board
			ResetBoard(boardSize);
			RedrawBoard();
		}
		
		// Define the form of the CGMT callback function
		private delegate void CGMTCallback(int x, int y);
		
		// A callback function to update the board state after MelkorWrapper.DllGenerateMove() has finished
		private void ThreadEndCallBack(int x, int y) {
			// When the thread has finished...
			
			// Check if the computer has run out of time!
			if(mode!=Mode.GameInProgress)
				return;
			
			// Re-enable certain menu items disable by ComputerGenerateMove()
			optionsMenu.Enabled = true;
			newMenuItem.Enabled = true;
			openMenuItem.Enabled = true;
			saveMenuItem.Enabled = true;
			saveAsMenuItem.Enabled = true;
			passButton.Enabled = true;
			undoButton.Enabled = true;
			scoreBlackMovesButton.Enabled = true;
			scoreWhiteMovesButton.Enabled = true;
			clearScoredMovesButton.Enabled = true;
			
			computerGeneratingMove = false;			
			logWindow.writeLine("Computer plays "+x+","+y);

			if(x==-1 && y==-1)
				MessageBox.Show("Computer has passed.");
			
			// Do post move things, if runAfterMove() returns false it means
			// two passes have happened, therefore end of game
			if(!runAfterMove(x, y))
				return;
						
			// Start the timer for the opponent now
			if(timedGame)
				oneSecondTimer.Start();
		}

		// A wrapper class to run MelkorWrapper.DllGenerateMove() in a separate thread.
		// Includes a callback delegate argument in the constructor for a CGMTCallback function
		private class ComputerGenerateMoveThread {
			public int boardX;
			public int boardY;
			public int nextToPlay;
			
			private CGMTCallback callback;
			
			public ComputerGenerateMoveThread(int toPlay, CGMTCallback callbackDelegate) {
				boardX = 0;
				boardY = 0;
				nextToPlay = toPlay;
				callback = callbackDelegate;
			}
			
			public void ThreadProc() {
				MelkorWrapper.DllGenerateMove(ref boardX, ref boardY, nextToPlay);
				callback(boardX, boardY);
			}
		}
		
		private void HumanPlayMove(int boardX, int boardY) {
			if(computerGeneratingMove) {
				MessageBox.Show("Computer is thinking...");
				return;
			}
			// Try the move and beep if it's illegal
			if ( MelkorWrapper.DllPlayMove(boardX, boardY, nextToPlay) == 0) {
				Beep(600, 100);
				return;
			}

			// Stop the timer
			if(timedGame)
				oneSecondTimer.Stop();
			
			logWindow.writeLine("Human plays "+boardX+","+boardY);

			// Do post move things, if runAfterMove() returns false it means
			// two passes have happened, therefore end of game
			if(!runAfterMove(boardX, boardY))
				return;			
			
			// Is it the computers turn to play?
			if ((nextToPlay == MelkorWrapper.BLACK && blackPlayer == COMPUTER) ||
			    (nextToPlay == MelkorWrapper.WHITE && whitePlayer == COMPUTER))
				ComputerGenerateMove();
			else if(timedGame)
				oneSecondTimer.Start();
		}
		
		private void ComputerGenerateMove() {
			/// TODO Add a 'thinking...' animation so the user knows what's happening
			
			// Inform Melkor of it's current time settings
			if(blackPlayer==COMPUTER)
				MelkorWrapper.DllSetCurrentTimeSettingsBlack(currentMainTimeBlack, currentByoyomiTimeBlack,
				                                        currentByoyomiStonesBlack);
			else
				MelkorWrapper.DllSetCurrentTimeSettingsWhite(currentMainTimeWhite, currentByoyomiTimeWhite,
				                                        currentByoyomiStonesWhite);
			
			ComputerGenerateMoveThread cgmt = new ComputerGenerateMoveThread(nextToPlay, new CGMTCallback(ThreadEndCallBack));
			computerMoveGeneratingThread = new Thread(new ThreadStart(cgmt.ThreadProc));
			
			// Disable certain menus - to be re-enabled in ThreadEndCallBack()
			optionsMenu.Enabled = false;
			newMenuItem.Enabled = false;
			openMenuItem.Enabled = false;
			saveMenuItem.Enabled = false;
			saveAsMenuItem.Enabled = false;
			passButton.Enabled = false;
			undoButton.Enabled = false;
			scoreBlackMovesButton.Enabled = false;
			scoreWhiteMovesButton.Enabled = false;
			clearScoredMovesButton.Enabled = false;
			
			computerGeneratingMove = true;

			// Start the timer
			if(timedGame)
				oneSecondTimer.Start();
			
			// Start Melkor generating it's next move
			computerMoveGeneratingThread.Start();
			Thread.Sleep(0);
		}

		// This function needs to be run after each move is placed during a game
		// It returns false if the end of the game has been signalled by 2 passes
		// and runs endGame() to update the program state.
		private bool runAfterMove(int x, int y) {
			if(timedGame)
				updateByoyomi();			
			resetBoardStatusArrays();
			updateCapturedStones();
			RedrawBoard();
			if(nextToPlay == MelkorWrapper.BLACK)
				changeTurn(MelkorWrapper.WHITE);
			else
				changeTurn(MelkorWrapper.BLACK);

			if(x==-1 && y==-1) {
				if(lastMovePass) {
					if(timedGame)
						oneSecondTimer.Stop();
					endGame(false);
					lastMovePass = true;
					return false;
				}
				lastMovePass = true;
			}
			else
				lastMovePass = false;
			return true;
		}
		
		// Change the turn to newNextTurnToPlay and update the GUI
		private void changeTurn(int newNextToPlay) {
			nextToPlay = newNextToPlay;

			// Fade whoevers turn it isn't!
			ColorMatrix clrMatrix = new ColorMatrix();
			clrMatrix.Matrix00 = 1.0f;			// Red
			clrMatrix.Matrix11 = 1.0f;			// Green
			clrMatrix.Matrix22 = 1.0f;			// Blue
			clrMatrix.Matrix33 = 0.5f;			// Alpha
			clrMatrix.Matrix44 = 1.0f;			// w (?)
			ImageAttributes imgAttr = new ImageAttributes();
			imgAttr.SetColorMatrix(clrMatrix, ColorMatrixFlag.Default, ColorAdjustType.Bitmap);
			
			Graphics g = null;
			// Create correct resolution and sized images for the labels if they haven't been initialised yet
			if(blackLabel.Image==null)
				blackLabel.Image = new Bitmap(largeBlackStone, new Size(blackLabel.Width, blackLabel.Height));
			if(whiteLabel.Image==null)
				whiteLabel.Image = new Bitmap(largeWhiteStone, new Size(whiteLabel.Width, whiteLabel.Height));
			// Wipe the old images otherwise our new ones won't show properly
			Brush labelBrush = new SolidBrush(blackLabel.BackColor);
			g = Graphics.FromImage(blackLabel.Image);
			g.FillRectangle(labelBrush, 0, 0, blackLabel.Image.Width, blackLabel.Image.Height);
			g.Dispose();
			g = Graphics.FromImage(whiteLabel.Image);
			g.FillRectangle(labelBrush, 0, 0, whiteLabel.Image.Width, whiteLabel.Image.Height);
			g.Dispose();
			if(nextToPlay==MelkorWrapper.BLACK) {
				g = Graphics.FromImage(blackLabel.Image);
				g.DrawImage(largeBlackStone, new Rectangle(0, 0, blackLabel.Image.Width, blackLabel.Image.Height));
				g.Dispose();
				// And fade the other one
				g = Graphics.FromImage(whiteLabel.Image);
				g.DrawImage(largeWhiteStone, new Rectangle(0, 0, whiteLabel.Image.Width, whiteLabel.Image.Height),
				            0, 0, largeWhiteStone.Width, largeWhiteStone.Height, GraphicsUnit.Pixel, imgAttr);
				g.Dispose();
			}
			else {
				g = Graphics.FromImage(whiteLabel.Image);
				g.DrawImage(largeWhiteStone, new Rectangle(0, 0, whiteLabel.Image.Width, whiteLabel.Image.Height));
				g.Dispose();
				// And fade the other one
				g = Graphics.FromImage(blackLabel.Image);
				g.DrawImage(largeBlackStone, new Rectangle(0, 0, blackLabel.Image.Width, blackLabel.Image.Height),
				            0, 0, largeBlackStone.Width, largeBlackStone.Height, GraphicsUnit.Pixel, imgAttr);
				g.Dispose();
			}
			blackLabel.Refresh();
			whiteLabel.Refresh();
		}

		#region Board drawing functions
		
		private void RedrawBoard() {
			// Update our copy of the board
			MelkorWrapper.DllGetBoard(board, boardSize);

			PaintBoard();
		}
		
		private void PaintBoard() {

			// Skip drawing if the user can't see anything - also avoids
			// a crash when trying to draw a board < 0 pixels in size
			if(boardPictureBox.ClientSize.Width<=20 || boardPictureBox.ClientSize.Height<=20)
				return;
			
			boardImageWidth = boardPictureBox.ClientSize.Width;
			boardImageHeight = boardPictureBox.ClientSize.Height;
			
			// Make sure we get a square board
			if(boardImageWidth!=boardImageHeight) {
				if(boardImageWidth<boardImageHeight) 
					boardImageHeight = boardImageWidth;
				else
					boardImageWidth = boardImageHeight;
			}

			// Set the border to a 20th of the available image width
			border = ((float)boardImageWidth / 20);

			// Set the grid step size
			gridStep = ((float)boardImageWidth - border*2) / ((float)boardSize);
			//logWindow.writeLine("boardImageWidth="+boardImageWidth+" border="+border+" gridStep="+gridStep);

			// Recalculate the boardImageWidth and height now we know the rounded gridstep
    		boardImageWidth = (int)(((float)boardSize)*gridStep + border*2);
    		boardImageHeight = boardImageWidth;
			// Check that the user can still see a board at all
			if(boardImageWidth<=20 || boardImageHeight<=20)
				return;
			
    		//logWindow.writeLine("new boardImageWidth="+boardImageWidth);
    		if(boardPictureBox.Image!=null)
				boardPictureBox.Image.Dispose();
			boardPictureBox.Image = new Bitmap(boardImageWidth, boardImageHeight);
			
			// Draw the gridlines and hoshii points
			Graphics g = null;
			g = Graphics.FromImage(boardPictureBox.Image);
			
			// Draw the board background
			Image boardBackgroundImage = new Bitmap(boardImageWidth, boardImageHeight);
			boardBackgroundImage = (Bitmap)boardBackground.Clone();
			g.DrawImage(boardBackgroundImage, 0, 0, boardImageWidth, boardImageHeight);
			
			// Add coordinates around the board edge
			// Write text between 0 and 'gridstep' pixels into the board
			Font font = new Font("Microsoft Sans Serif", gridStep/3, FontStyle.Bold);
			Brush blackBrush = new SolidBrush(Color.Black);
			Brush whiteBrush = new SolidBrush(Color.White);
			for(int number = 0;number < boardSize;number++) {
				// Draw numbers
				g.DrawString(Convert.ToString(boardSize-number), font, blackBrush, 0, border+gridStep*number+gridStep/4);
				// Draw the letters, noting to skip 'I'
				if(number>7)
					g.DrawString(Convert.ToString(Convert.ToChar(Convert.ToInt16('A')+number+1)), font, blackBrush, border+gridStep*number+gridStep/4, 0);
				else
					g.DrawString(Convert.ToString(Convert.ToChar(Convert.ToInt16('A')+number)), font, blackBrush, border+gridStep*number+gridStep/4, 0);
			}
			
			//float lineLength = boardSize*gridStep;
			Pen blackPen = new Pen(Color.Black, 2);
			for (int c = 0; c < boardSize; c++) {
				// Vertical lines
				g.DrawLine(blackPen, BoardToPBCoord(c), BoardToPBCoord(0), BoardToPBCoord(c), BoardToPBCoord(boardSize-1));
				// Horizontal lines
				g.DrawLine(blackPen, BoardToPBCoord(0), BoardToPBCoord(c), BoardToPBCoord(boardSize-1), BoardToPBCoord(c));
			}
			
			// Draw the hoshii points
			float hoshiiRadiusScale = 4.0f;
			if( boardSize == 19) {
				DrawEllipse(ref g, blackBrush, null, 3, 3, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 3, 9, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 3, 15, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 9, 3, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 9, 9, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 9, 15, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 15, 3, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 15, 9, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 15, 15, hoshiiRadiusScale, true, false);
			}
			else if(boardSize == 9) {
				DrawEllipse(ref g, blackBrush, null, 2, 2, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 2, 4, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 2, 6, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 4, 2, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 4, 4, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 4, 6, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 6, 2, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 6, 4, hoshiiRadiusScale, true, false);
				DrawEllipse(ref g, blackBrush, null, 6, 6, hoshiiRadiusScale, true, false);
			}
			
			// Draw the stones
			int size = boardSize * boardSize;
			float nnScoresScale = 2.0f;
			float territoryScale = 3.0f;
			for (int c = 0; c < size; c++) {
				// X and Y are board coordinates, use BoardToPBCoord() to convert to
				// PictureBox coordinate frame
				int x = c % boardSize;
				int y = c / boardSize;
				if( board[c] == MelkorWrapper.BLACK )
					DrawStone(ref g, x, y, MelkorWrapper.BLACK);
				else if( board[c] == MelkorWrapper.WHITE )
					DrawStone(ref g, x, y, MelkorWrapper.WHITE);
				
				// Mark dead stones as territory and empty spaces
				if(drawFadedStones[c] || board[c]==MelkorWrapper.EMPTY) {
					// Highlight this point if it's marked as territory
					if(blackTerritory[c])
						g.FillRectangle(blackBrush, BoardToPBCoord(x)-(float)gridStep/(territoryScale*2),
			             		  BoardToPBCoord(y)-(float)gridStep/(territoryScale*2),
			             		 (float)gridStep/territoryScale, (float)gridStep/territoryScale);
					else if(whiteTerritory[c])
						g.FillRectangle(whiteBrush, BoardToPBCoord(x)-(float)gridStep/(territoryScale*2),
			              		  BoardToPBCoord(y)-(float)gridStep/(territoryScale*2),
			              		 (float)gridStep/territoryScale, (float)gridStep/territoryScale);
				}
					
				// Draw the nnScores if required
				if(drawNNScores && nnScores[c] >= 0.0f) {
					Brush colouredBrush = new SolidBrush(Color.FromArgb(255, (int)(255 - (nnScores[c] * 255)), 0, (int)(nnScores[c] * 255)));
					DrawEllipse(ref g, colouredBrush, blackPen, x, y, nnScoresScale, true, true);
					colouredBrush.Dispose();
				}				
			}
			
			if (highlightLastMove) {
				float lastMoveScale = 2.0f;
				MelkorWrapper.DllGetLastMove(ref lastX, ref lastY);
				if (lastX != -1 && lastY != -1) {
					Pen lastMovePen;
					if (board[lastY * boardSize + lastX] == MelkorWrapper.BLACK)
						lastMovePen = new Pen(Color.White, 1);
					else
						lastMovePen = new Pen(Color.Black, 1);
					DrawEllipse(ref g, null, lastMovePen, lastX, lastY, lastMoveScale, false, true);
					lastMovePen.Dispose();
				}
			}
			
			drawNNScores = false;
			
			blackPen.Dispose();
			blackBrush.Dispose();
			whiteBrush.Dispose();
			g.Dispose();
			
			boardPictureBox.Refresh();
		}

		private void DrawStone(ref Graphics g, int x, int y, int colour) {

			float stoneRadiusScale = 1.0f;
			float topLeftX = BoardToPBCoord(x)-(float)gridStep/(stoneRadiusScale*2);
			float topLeftY = BoardToPBCoord(y)-(float)gridStep/(stoneRadiusScale*2);
			float width = (float)gridStep/stoneRadiusScale;
			float height = width;
			
			// Draw faded stones if requested - use alpha values, ImageAttributes?
			if(drawFadedStones[y*boardSize+x]) {
				ColorMatrix clrMatrix = new ColorMatrix();
				clrMatrix.Matrix00 = 1.0f;			// Red
				clrMatrix.Matrix11 = 1.0f;			// Green
				clrMatrix.Matrix22 = 1.0f;			// Blue
				clrMatrix.Matrix33 = 0.7f;			// Alpha
				clrMatrix.Matrix44 = 1.0f;			// w (?)
				ImageAttributes imgAttr = new ImageAttributes();
				imgAttr.SetColorMatrix(clrMatrix, ColorMatrixFlag.Default, ColorAdjustType.Bitmap);
				if(colour==MelkorWrapper.BLACK)
					g.DrawImage(largeBlackStone, new Rectangle((int)topLeftX, (int)topLeftY, (int)width, (int)height),
					            0, 0, largeBlackStone.Width, largeBlackStone.Height, GraphicsUnit.Pixel, imgAttr);
				else
					g.DrawImage(largeWhiteStone, new Rectangle((int)topLeftX, (int)topLeftY, (int)width, (int)height),
					            0, 0, largeWhiteStone.Width, largeWhiteStone.Height, GraphicsUnit.Pixel, imgAttr);
			}
			else {
				if(colour==MelkorWrapper.BLACK)
					g.DrawImage(largeBlackStone, topLeftX, topLeftY, width, height);
				else
					g.DrawImage(largeWhiteStone, topLeftX, topLeftY, width, height);
			}
		}
		
		private void DrawEllipse(ref Graphics g, Brush b, Pen p, int x, int y, 
		                         float radiusScale, bool fillEllipse, bool drawOutline) {
			if(fillEllipse && b!=null) {
				g.FillEllipse(b, 
			    	      BoardToPBCoord(x)-(float)gridStep/(radiusScale*2),
			              BoardToPBCoord(y)-(float)gridStep/(radiusScale*2),
			              (float)gridStep/radiusScale, (float)gridStep/radiusScale);
			}
			if(drawOutline && p!=null) {
				g.DrawEllipse(p,
			              BoardToPBCoord(x)-(float)gridStep/(radiusScale*2),
			              BoardToPBCoord(y)-(float)gridStep/(radiusScale*2),
			              (float)gridStep/radiusScale, (float)gridStep/radiusScale);
			}
		}
		
		// Reset the board to a new size, clear it and associated arrays
		// Also inform DLL we have done so, so it can update too
		private void ResetBoard(int size) {
			// Call the DLL function to set default board size
			boardSize = size;
			board = new int[boardSize*boardSize];
			nnScores = new float[boardSize*boardSize];
			drawFadedStones = new bool[boardSize*boardSize];
			blackTerritory = new bool[boardSize*boardSize];
			whiteTerritory = new bool[boardSize*boardSize];
			resetBoardStatusArrays();
			MelkorWrapper.DllSetBoardSize(boardSize);
		}

		// Reset the board to a new size, clear it and associated arrays
		// Don't tell DLL to reset
		private void ResetBoardDontUpdateDLL(int size) {
			// Call the DLL function to set default board size
			boardSize = size;
			board = new int[boardSize*boardSize];
			nnScores = new float[boardSize*boardSize];
			drawFadedStones = new bool[boardSize*boardSize];
			blackTerritory = new bool[boardSize*boardSize];
			whiteTerritory = new bool[boardSize*boardSize];
			resetBoardStatusArrays();
		}

		private void resetBoardStatusArrays() {
			for(int i=0;i<boardSize*boardSize;i++)
				nnScores[i] = 0.0f;
			ResetDeadStones();
			ResetTerritoryMarkers();
		}
		
		private void ResetDeadStones() {
			for(int i=0;i<boardSize*boardSize;i++)
				drawFadedStones[i] = false;
		}
		
		private void ResetTerritoryMarkers() {
			for(int i=0;i<boardSize*boardSize;i++) {
				blackTerritory[i] = false;
				whiteTerritory[i] = false;
			}
		}

		// Return the centre point of the corresponding board coordinate on the pictureBox image
		private float BoardToPBCoord(int c) {
			return border + (((float)c) * gridStep) + gridStep/2;
		}

		// Find the board coordinate given the centre point of the point on the pictureBox image
		private int PBCoordToBoard(float x) {
			return (int)((x - border)/gridStep);
		}

		#endregion
		
		[STAThread]
		public static void Main(string[] args)
		{
			Application.Run(new MainForm());
		}
		
		#region Windows Forms Designer generated code
		/// <summary>
		/// This method is required for Windows Forms designer support.
		/// Do not change the method contents inside the source code editor. The Forms designer might
		/// not be able to load this method if it was changed manually.
		/// </summary>
		private void InitializeComponent() {
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(MainForm));
			this.saveAsMenuItem = new System.Windows.Forms.MenuItem();
			this.scoreWhiteMovesButton = new System.Windows.Forms.Button();
			this.whiteLabel = new System.Windows.Forms.Label();
			this.panel1 = new System.Windows.Forms.Panel();
			this.blackTimeTextBox = new System.Windows.Forms.TextBox();
			this.scoreBlackMovesButton = new System.Windows.Forms.Button();
			this.optionsMenu = new System.Windows.Forms.MenuItem();
			this.saveMenuItem = new System.Windows.Forms.MenuItem();
			this.gtpLabel = new System.Windows.Forms.Label();
			this.boardPictureBox = new System.Windows.Forms.PictureBox();
			this.gameControlLabel = new System.Windows.Forms.Label();
			this.blackByoyomiTextBox = new System.Windows.Forms.TextBox();
			this.openMenuItem = new System.Windows.Forms.MenuItem();
			this.oneSecondTimer = new System.Windows.Forms.Timer(this.components);
			this.backToStartButton = new System.Windows.Forms.Button();
			this.clearScoredMovesButton = new System.Windows.Forms.Button();
			this.undoButton = new System.Windows.Forms.Button();
			this.whiteTimeTextBox = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.changePlayerMenuItem = new System.Windows.Forms.MenuItem();
			this.gtpArgumentsTextBox = new System.Windows.Forms.TextBox();
			this.aboutMenuItem = new System.Windows.Forms.MenuItem();
			this.panel4 = new System.Windows.Forms.Panel();
			this.customiseMenuItem = new System.Windows.Forms.MenuItem();
			this.viewMenu = new System.Windows.Forms.MenuItem();
			this.gameInformationLabel = new System.Windows.Forms.Label();
			this.resultTextBox = new System.Windows.Forms.TextBox();
			this.panel3 = new System.Windows.Forms.Panel();
			this.whiteCapturedTextBox = new System.Windows.Forms.TextBox();
			this.gtpCommandsComboBox = new System.Windows.Forms.ComboBox();
			this.timeLabel = new System.Windows.Forms.Label();
			this.helpMenu = new System.Windows.Forms.MenuItem();
			this.fileMenu = new System.Windows.Forms.MenuItem();
			this.logWindowMenuItem = new System.Windows.Forms.MenuItem();
			this.backStepButton = new System.Windows.Forms.Button();
			this.blackCapturedTextBox = new System.Windows.Forms.TextBox();
			this.blackLabel = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.backButton = new System.Windows.Forms.Button();
			this.capturedLabel = new System.Windows.Forms.Label();
			this.newMenuItem = new System.Windows.Forms.MenuItem();
			this.loadNeuralNetworkMenuItem = new System.Windows.Forms.MenuItem();
			this.runGTPCommandButton = new System.Windows.Forms.Button();
			this.whiteByoyomiTextBox = new System.Windows.Forms.TextBox();
			this.panel2 = new System.Windows.Forms.Panel();
			this.mainMenu1 = new System.Windows.Forms.MainMenu();
			this.forwardStepButton = new System.Windows.Forms.Button();
			this.menuItem6 = new System.Windows.Forms.MenuItem();
			this.scoreButton = new System.Windows.Forms.Button();
			this.forwardToEndButton = new System.Windows.Forms.Button();
			this.passButton = new System.Windows.Forms.Button();
			this.playerMenu = new System.Windows.Forms.MenuItem();
			this.exitMenuItem = new System.Windows.Forms.MenuItem();
			this.newPlayerMenuItem = new System.Windows.Forms.MenuItem();
			this.forwardButton = new System.Windows.Forms.Button();
			this.nnScoreTextBox = new System.Windows.Forms.TextBox();
			this.sgfStepTextBox = new System.Windows.Forms.TextBox();
			this.searchSettingsMenuItem = new System.Windows.Forms.MenuItem();
			this.panel1.SuspendLayout();
			this.panel4.SuspendLayout();
			this.panel3.SuspendLayout();
			this.panel2.SuspendLayout();
			this.SuspendLayout();
			// 
			// saveAsMenuItem
			// 
			this.saveAsMenuItem.Index = 3;
			this.saveAsMenuItem.Text = "Save As...";
			this.saveAsMenuItem.Click += new System.EventHandler(this.SaveAsMenuItemClick);
			// 
			// scoreWhiteMovesButton
			// 
			this.scoreWhiteMovesButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.scoreWhiteMovesButton.Location = new System.Drawing.Point(672, 696);
			this.scoreWhiteMovesButton.Name = "scoreWhiteMovesButton";
			this.scoreWhiteMovesButton.Size = new System.Drawing.Size(120, 23);
			this.scoreWhiteMovesButton.TabIndex = 9;
			this.scoreWhiteMovesButton.Text = "Score White Moves";
			this.scoreWhiteMovesButton.Click += new System.EventHandler(this.ScoreWhiteMovesButtonClick);
			// 
			// whiteLabel
			// 
			this.whiteLabel.BackColor = System.Drawing.SystemColors.ControlDark;
			this.whiteLabel.Location = new System.Drawing.Point(160, 48);
			this.whiteLabel.Name = "whiteLabel";
			this.whiteLabel.Size = new System.Drawing.Size(32, 32);
			this.whiteLabel.TabIndex = 1;
			// 
			// panel1
			// 
			this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.panel1.Controls.Add(this.resultTextBox);
			this.panel1.Controls.Add(this.whiteByoyomiTextBox);
			this.panel1.Controls.Add(this.blackByoyomiTextBox);
			this.panel1.Controls.Add(this.label2);
			this.panel1.Controls.Add(this.whiteCapturedTextBox);
			this.panel1.Controls.Add(this.blackCapturedTextBox);
			this.panel1.Controls.Add(this.whiteTimeTextBox);
			this.panel1.Controls.Add(this.blackTimeTextBox);
			this.panel1.Controls.Add(this.capturedLabel);
			this.panel1.Controls.Add(this.timeLabel);
			this.panel1.Controls.Add(this.blackLabel);
			this.panel1.Controls.Add(this.whiteLabel);
			this.panel1.Controls.Add(this.gameInformationLabel);
			this.panel1.Location = new System.Drawing.Point(672, 32);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(232, 184);
			this.panel1.TabIndex = 3;
			// 
			// blackTimeTextBox
			// 
			this.blackTimeTextBox.Location = new System.Drawing.Point(80, 88);
			this.blackTimeTextBox.Name = "blackTimeTextBox";
			this.blackTimeTextBox.ReadOnly = true;
			this.blackTimeTextBox.Size = new System.Drawing.Size(56, 20);
			this.blackTimeTextBox.TabIndex = 5;
			this.blackTimeTextBox.TabStop = false;
			this.blackTimeTextBox.Text = "0";
			// 
			// scoreBlackMovesButton
			// 
			this.scoreBlackMovesButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.scoreBlackMovesButton.Location = new System.Drawing.Point(672, 664);
			this.scoreBlackMovesButton.Name = "scoreBlackMovesButton";
			this.scoreBlackMovesButton.Size = new System.Drawing.Size(120, 23);
			this.scoreBlackMovesButton.TabIndex = 6;
			this.scoreBlackMovesButton.Text = "Score Black Moves";
			this.scoreBlackMovesButton.Click += new System.EventHandler(this.ScoreBlackMovesButtonClick);
			// 
			// optionsMenu
			// 
			this.optionsMenu.Index = 2;
			this.optionsMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
						this.searchSettingsMenuItem,
						this.loadNeuralNetworkMenuItem,
						this.customiseMenuItem});
			this.optionsMenu.Text = "&Options";
			// 
			// saveMenuItem
			// 
			this.saveMenuItem.Index = 2;
			this.saveMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlS;
			this.saveMenuItem.Text = "&Save";
			// 
			// gtpLabel
			// 
			this.gtpLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.gtpLabel.Location = new System.Drawing.Point(8, 8);
			this.gtpLabel.Name = "gtpLabel";
			this.gtpLabel.Size = new System.Drawing.Size(40, 24);
			this.gtpLabel.TabIndex = 0;
			this.gtpLabel.Text = "GTP";
			// 
			// boardPictureBox
			// 
			this.boardPictureBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
						| System.Windows.Forms.AnchorStyles.Left) 
						| System.Windows.Forms.AnchorStyles.Right)));
			this.boardPictureBox.Location = new System.Drawing.Point(16, 32);
			this.boardPictureBox.Name = "boardPictureBox";
			this.boardPictureBox.Size = new System.Drawing.Size(640, 768);
			this.boardPictureBox.TabIndex = 2;
			this.boardPictureBox.TabStop = false;
			this.boardPictureBox.MouseUp += new System.Windows.Forms.MouseEventHandler(this.BoardPictureBoxMouseUp);
			this.boardPictureBox.MouseDown += new System.Windows.Forms.MouseEventHandler(this.BoardPictureBoxMouseDown);
			this.boardPictureBox.Layout += new System.Windows.Forms.LayoutEventHandler(this.BoardPictureBoxLayout);
			// 
			// gameControlLabel
			// 
			this.gameControlLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.gameControlLabel.Location = new System.Drawing.Point(8, 8);
			this.gameControlLabel.Name = "gameControlLabel";
			this.gameControlLabel.Size = new System.Drawing.Size(80, 23);
			this.gameControlLabel.TabIndex = 0;
			this.gameControlLabel.Text = "Game Control";
			// 
			// blackByoyomiTextBox
			// 
			this.blackByoyomiTextBox.Location = new System.Drawing.Point(80, 120);
			this.blackByoyomiTextBox.Name = "blackByoyomiTextBox";
			this.blackByoyomiTextBox.ReadOnly = true;
			this.blackByoyomiTextBox.Size = new System.Drawing.Size(56, 20);
			this.blackByoyomiTextBox.TabIndex = 10;
			this.blackByoyomiTextBox.TabStop = false;
			this.blackByoyomiTextBox.Text = "0";
			// 
			// openMenuItem
			// 
			this.openMenuItem.Index = 1;
			this.openMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlO;
			this.openMenuItem.Text = "&Open...";
			this.openMenuItem.Click += new System.EventHandler(this.OpenMenuItemClick);
			// 
			// oneSecondTimer
			// 
			this.oneSecondTimer.Interval = 1000;
			this.oneSecondTimer.Tick += new System.EventHandler(this.OneSecondTimerTick);
			// 
			// backToStartButton
			// 
			this.backToStartButton.Enabled = false;
			this.backToStartButton.Image = ((System.Drawing.Image)(resources.GetObject("backToStartButton.Image")));
			this.backToStartButton.Location = new System.Drawing.Point(8, 8);
			this.backToStartButton.Name = "backToStartButton";
			this.backToStartButton.Size = new System.Drawing.Size(24, 24);
			this.backToStartButton.TabIndex = 1;
			this.backToStartButton.Click += new System.EventHandler(this.BackToStartButtonClick);
			// 
			// clearScoredMovesButton
			// 
			this.clearScoredMovesButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.clearScoredMovesButton.Location = new System.Drawing.Point(672, 728);
			this.clearScoredMovesButton.Name = "clearScoredMovesButton";
			this.clearScoredMovesButton.Size = new System.Drawing.Size(120, 23);
			this.clearScoredMovesButton.TabIndex = 10;
			this.clearScoredMovesButton.Text = "Clear Scored Moves";
			this.clearScoredMovesButton.Click += new System.EventHandler(this.ClearScoredMovesButtonClick);
			// 
			// undoButton
			// 
			this.undoButton.Location = new System.Drawing.Point(16, 96);
			this.undoButton.Name = "undoButton";
			this.undoButton.TabIndex = 2;
			this.undoButton.Text = "Undo";
			this.undoButton.Click += new System.EventHandler(this.UndoButtonClick);
			// 
			// whiteTimeTextBox
			// 
			this.whiteTimeTextBox.Location = new System.Drawing.Point(152, 88);
			this.whiteTimeTextBox.Name = "whiteTimeTextBox";
			this.whiteTimeTextBox.ReadOnly = true;
			this.whiteTimeTextBox.Size = new System.Drawing.Size(56, 20);
			this.whiteTimeTextBox.TabIndex = 6;
			this.whiteTimeTextBox.TabStop = false;
			this.whiteTimeTextBox.Text = "0";
			// 
			// label2
			// 
			this.label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.label2.Location = new System.Drawing.Point(8, 120);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(64, 16);
			this.label2.TabIndex = 9;
			this.label2.Text = "Byo-Yomi";
			// 
			// changePlayerMenuItem
			// 
			this.changePlayerMenuItem.Index = 1;
			this.changePlayerMenuItem.Text = "&Change Player...";
			// 
			// gtpArgumentsTextBox
			// 
			this.gtpArgumentsTextBox.Enabled = false;
			this.gtpArgumentsTextBox.Location = new System.Drawing.Point(8, 112);
			this.gtpArgumentsTextBox.Name = "gtpArgumentsTextBox";
			this.gtpArgumentsTextBox.Size = new System.Drawing.Size(208, 20);
			this.gtpArgumentsTextBox.TabIndex = 3;
			this.gtpArgumentsTextBox.Text = "";
			// 
			// aboutMenuItem
			// 
			this.aboutMenuItem.Index = 0;
			this.aboutMenuItem.Text = "&About";
			this.aboutMenuItem.Click += new System.EventHandler(this.AboutMenuItemClick);
			// 
			// panel4
			// 
			this.panel4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.panel4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.panel4.Controls.Add(this.runGTPCommandButton);
			this.panel4.Controls.Add(this.gtpArgumentsTextBox);
			this.panel4.Controls.Add(this.label1);
			this.panel4.Controls.Add(this.gtpCommandsComboBox);
			this.panel4.Controls.Add(this.gtpLabel);
			this.panel4.Location = new System.Drawing.Point(672, 464);
			this.panel4.Name = "panel4";
			this.panel4.Size = new System.Drawing.Size(232, 184);
			this.panel4.TabIndex = 5;
			// 
			// customiseMenuItem
			// 
			this.customiseMenuItem.Index = 2;
			this.customiseMenuItem.Text = "&Customise...";
			// 
			// viewMenu
			// 
			this.viewMenu.Index = 1;
			this.viewMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
						this.logWindowMenuItem});
			this.viewMenu.Text = "&View";
			// 
			// gameInformationLabel
			// 
			this.gameInformationLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.gameInformationLabel.Location = new System.Drawing.Point(8, 8);
			this.gameInformationLabel.Name = "gameInformationLabel";
			this.gameInformationLabel.TabIndex = 0;
			this.gameInformationLabel.Text = "Game Information";
			// 
			// resultTextBox
			// 
			this.resultTextBox.Location = new System.Drawing.Point(128, 8);
			this.resultTextBox.Name = "resultTextBox";
			this.resultTextBox.ReadOnly = true;
			this.resultTextBox.Size = new System.Drawing.Size(56, 20);
			this.resultTextBox.TabIndex = 12;
			this.resultTextBox.TabStop = false;
			this.resultTextBox.Text = "";
			// 
			// panel3
			// 
			this.panel3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.panel3.Controls.Add(this.forwardToEndButton);
			this.panel3.Controls.Add(this.forwardStepButton);
			this.panel3.Controls.Add(this.forwardButton);
			this.panel3.Controls.Add(this.backButton);
			this.panel3.Controls.Add(this.backStepButton);
			this.panel3.Controls.Add(this.backToStartButton);
			this.panel3.Controls.Add(this.sgfStepTextBox);
			this.panel3.Location = new System.Drawing.Point(8, 144);
			this.panel3.Name = "panel3";
			this.panel3.Size = new System.Drawing.Size(216, 48);
			this.panel3.TabIndex = 4;
			// 
			// whiteCapturedTextBox
			// 
			this.whiteCapturedTextBox.Location = new System.Drawing.Point(152, 152);
			this.whiteCapturedTextBox.Name = "whiteCapturedTextBox";
			this.whiteCapturedTextBox.ReadOnly = true;
			this.whiteCapturedTextBox.Size = new System.Drawing.Size(56, 20);
			this.whiteCapturedTextBox.TabIndex = 8;
			this.whiteCapturedTextBox.TabStop = false;
			this.whiteCapturedTextBox.Text = "0";
			// 
			// gtpCommandsComboBox
			// 
			this.gtpCommandsComboBox.Enabled = false;
			this.gtpCommandsComboBox.Location = new System.Drawing.Point(8, 48);
			this.gtpCommandsComboBox.Name = "gtpCommandsComboBox";
			this.gtpCommandsComboBox.Size = new System.Drawing.Size(208, 21);
			this.gtpCommandsComboBox.TabIndex = 1;
			this.gtpCommandsComboBox.Text = "GTPCommands";
			// 
			// timeLabel
			// 
			this.timeLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.timeLabel.Location = new System.Drawing.Point(8, 88);
			this.timeLabel.Name = "timeLabel";
			this.timeLabel.Size = new System.Drawing.Size(64, 16);
			this.timeLabel.TabIndex = 3;
			this.timeLabel.Text = "Main Time";
			// 
			// helpMenu
			// 
			this.helpMenu.Index = 4;
			this.helpMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
						this.aboutMenuItem});
			this.helpMenu.Text = "&Help";
			// 
			// fileMenu
			// 
			this.fileMenu.Index = 0;
			this.fileMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
						this.newMenuItem,
						this.openMenuItem,
						this.saveMenuItem,
						this.saveAsMenuItem,
						this.menuItem6,
						this.exitMenuItem});
			this.fileMenu.Text = "&File";
			// 
			// logWindowMenuItem
			// 
			this.logWindowMenuItem.Index = 0;
			this.logWindowMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlL;
			this.logWindowMenuItem.Text = "Log Window";
			this.logWindowMenuItem.Click += new System.EventHandler(this.LogWindowMenuItemClick);
			// 
			// backStepButton
			// 
			this.backStepButton.Enabled = false;
			this.backStepButton.Image = ((System.Drawing.Image)(resources.GetObject("backStepButton.Image")));
			this.backStepButton.Location = new System.Drawing.Point(32, 8);
			this.backStepButton.Name = "backStepButton";
			this.backStepButton.Size = new System.Drawing.Size(24, 24);
			this.backStepButton.TabIndex = 2;
			this.backStepButton.Click += new System.EventHandler(this.BackStepButtonClick);
			// 
			// blackCapturedTextBox
			// 
			this.blackCapturedTextBox.Location = new System.Drawing.Point(80, 152);
			this.blackCapturedTextBox.Name = "blackCapturedTextBox";
			this.blackCapturedTextBox.ReadOnly = true;
			this.blackCapturedTextBox.Size = new System.Drawing.Size(56, 20);
			this.blackCapturedTextBox.TabIndex = 7;
			this.blackCapturedTextBox.TabStop = false;
			this.blackCapturedTextBox.Text = "0";
			// 
			// blackLabel
			// 
			this.blackLabel.BackColor = System.Drawing.SystemColors.ControlDark;
			this.blackLabel.Location = new System.Drawing.Point(88, 48);
			this.blackLabel.Name = "blackLabel";
			this.blackLabel.Size = new System.Drawing.Size(32, 32);
			this.blackLabel.TabIndex = 2;
			// 
			// label1
			// 
			this.label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.label1.Location = new System.Drawing.Point(8, 80);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(88, 24);
			this.label1.TabIndex = 2;
			this.label1.Text = "GTP Arguments";
			// 
			// backButton
			// 
			this.backButton.Enabled = false;
			this.backButton.Image = ((System.Drawing.Image)(resources.GetObject("backButton.Image")));
			this.backButton.Location = new System.Drawing.Point(56, 8);
			this.backButton.Name = "backButton";
			this.backButton.Size = new System.Drawing.Size(24, 24);
			this.backButton.TabIndex = 3;
			this.backButton.Click += new System.EventHandler(this.BackButtonClick);
			// 
			// capturedLabel
			// 
			this.capturedLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.capturedLabel.Location = new System.Drawing.Point(8, 152);
			this.capturedLabel.Name = "capturedLabel";
			this.capturedLabel.Size = new System.Drawing.Size(64, 16);
			this.capturedLabel.TabIndex = 4;
			this.capturedLabel.Text = "Captured";
			// 
			// newMenuItem
			// 
			this.newMenuItem.Index = 0;
			this.newMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlN;
			this.newMenuItem.Text = "&New...";
			this.newMenuItem.Click += new System.EventHandler(this.NewMenuItemClick);
			// 
			// loadNeuralNetworkMenuItem
			// 
			this.loadNeuralNetworkMenuItem.Index = 1;
			this.loadNeuralNetworkMenuItem.Text = "&Load Neural Network...";
			// 
			// runGTPCommandButton
			// 
			this.runGTPCommandButton.Enabled = false;
			this.runGTPCommandButton.Location = new System.Drawing.Point(56, 144);
			this.runGTPCommandButton.Name = "runGTPCommandButton";
			this.runGTPCommandButton.Size = new System.Drawing.Size(120, 23);
			this.runGTPCommandButton.TabIndex = 4;
			this.runGTPCommandButton.Text = "Run GTP Command";
			// 
			// whiteByoyomiTextBox
			// 
			this.whiteByoyomiTextBox.Location = new System.Drawing.Point(152, 120);
			this.whiteByoyomiTextBox.Name = "whiteByoyomiTextBox";
			this.whiteByoyomiTextBox.ReadOnly = true;
			this.whiteByoyomiTextBox.Size = new System.Drawing.Size(56, 20);
			this.whiteByoyomiTextBox.TabIndex = 11;
			this.whiteByoyomiTextBox.TabStop = false;
			this.whiteByoyomiTextBox.Text = "0";
			// 
			// panel2
			// 
			this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.panel2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.panel2.Controls.Add(this.panel3);
			this.panel2.Controls.Add(this.scoreButton);
			this.panel2.Controls.Add(this.undoButton);
			this.panel2.Controls.Add(this.passButton);
			this.panel2.Controls.Add(this.gameControlLabel);
			this.panel2.Location = new System.Drawing.Point(672, 240);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(232, 208);
			this.panel2.TabIndex = 4;
			// 
			// mainMenu1
			// 
			this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
						this.fileMenu,
						this.viewMenu,
						this.optionsMenu,
						this.playerMenu,
						this.helpMenu});
			// 
			// forwardStepButton
			// 
			this.forwardStepButton.Enabled = false;
			this.forwardStepButton.Image = ((System.Drawing.Image)(resources.GetObject("forwardStepButton.Image")));
			this.forwardStepButton.Location = new System.Drawing.Point(104, 8);
			this.forwardStepButton.Name = "forwardStepButton";
			this.forwardStepButton.Size = new System.Drawing.Size(24, 24);
			this.forwardStepButton.TabIndex = 5;
			this.forwardStepButton.Click += new System.EventHandler(this.ForwardStepButtonClick);
			// 
			// menuItem6
			// 
			this.menuItem6.Index = 4;
			this.menuItem6.Text = "-";
			// 
			// scoreButton
			// 
			this.scoreButton.Location = new System.Drawing.Point(120, 56);
			this.scoreButton.Name = "scoreButton";
			this.scoreButton.TabIndex = 3;
			this.scoreButton.Text = "Score";
			this.scoreButton.Click += new System.EventHandler(this.ScoreButtonClick);
			// 
			// forwardToEndButton
			// 
			this.forwardToEndButton.Enabled = false;
			this.forwardToEndButton.Image = ((System.Drawing.Image)(resources.GetObject("forwardToEndButton.Image")));
			this.forwardToEndButton.Location = new System.Drawing.Point(128, 8);
			this.forwardToEndButton.Name = "forwardToEndButton";
			this.forwardToEndButton.Size = new System.Drawing.Size(24, 24);
			this.forwardToEndButton.TabIndex = 6;
			this.forwardToEndButton.Click += new System.EventHandler(this.ForwardToEndButtonClick);
			// 
			// passButton
			// 
			this.passButton.Location = new System.Drawing.Point(16, 56);
			this.passButton.Name = "passButton";
			this.passButton.TabIndex = 1;
			this.passButton.Text = "Pass";
			this.passButton.Click += new System.EventHandler(this.PassButtonClick);
			// 
			// playerMenu
			// 
			this.playerMenu.Index = 3;
			this.playerMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
						this.newPlayerMenuItem,
						this.changePlayerMenuItem});
			this.playerMenu.Text = "&Player";
			// 
			// exitMenuItem
			// 
			this.exitMenuItem.Index = 5;
			this.exitMenuItem.Text = "E&xit";
			this.exitMenuItem.Click += new System.EventHandler(this.ExitMenuItemClick);
			// 
			// newPlayerMenuItem
			// 
			this.newPlayerMenuItem.Index = 0;
			this.newPlayerMenuItem.Text = "&New Player...";
			// 
			// forwardButton
			// 
			this.forwardButton.Enabled = false;
			this.forwardButton.Image = ((System.Drawing.Image)(resources.GetObject("forwardButton.Image")));
			this.forwardButton.Location = new System.Drawing.Point(80, 8);
			this.forwardButton.Name = "forwardButton";
			this.forwardButton.Size = new System.Drawing.Size(24, 24);
			this.forwardButton.TabIndex = 4;
			this.forwardButton.Click += new System.EventHandler(this.ForwardButtonClick);
			// 
			// nnScoreTextBox
			// 
			this.nnScoreTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.nnScoreTextBox.Location = new System.Drawing.Point(800, 664);
			this.nnScoreTextBox.Name = "nnScoreTextBox";
			this.nnScoreTextBox.ReadOnly = true;
			this.nnScoreTextBox.TabIndex = 7;
			this.nnScoreTextBox.TabStop = false;
			this.nnScoreTextBox.Text = "";
			// 
			// sgfStepTextBox
			// 
			this.sgfStepTextBox.Enabled = false;
			this.sgfStepTextBox.Location = new System.Drawing.Point(168, 8);
			this.sgfStepTextBox.Name = "sgfStepTextBox";
			this.sgfStepTextBox.Size = new System.Drawing.Size(32, 20);
			this.sgfStepTextBox.TabIndex = 0;
			this.sgfStepTextBox.Text = "5";
			// 
			// searchSettingsMenuItem
			// 
			this.searchSettingsMenuItem.Index = 0;
			this.searchSettingsMenuItem.Text = "&Search Settings...";
			this.searchSettingsMenuItem.Click += new System.EventHandler(this.DisplaySearchSettings);
			// 
			// MainForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.AutoScroll = true;
			this.ClientSize = new System.Drawing.Size(912, 813);
			this.Controls.Add(this.clearScoredMovesButton);
			this.Controls.Add(this.scoreWhiteMovesButton);
			this.Controls.Add(this.nnScoreTextBox);
			this.Controls.Add(this.scoreBlackMovesButton);
			this.Controls.Add(this.panel4);
			this.Controls.Add(this.panel2);
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.boardPictureBox);
			this.Menu = this.mainMenu1;
			this.Name = "MainForm";
			this.Text = "Melkor .NET GUI";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.MainFormClosing);
			this.panel1.ResumeLayout(false);
			this.panel4.ResumeLayout(false);
			this.panel3.ResumeLayout(false);
			this.panel2.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		#endregion
			
		void BoardPictureBoxMouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			// Check that the user started somewhere on the board...
			if(!mouseDownOnBoard)
				return;
			mouseDownOnBoard = false;
			
			int boardX, boardY;

			/// TODO For some reason this allows the user to click in the left-hand border
			/// (the coords) and still place a stone...
			boardX = PBCoordToBoard(e.X);
			boardY = PBCoordToBoard(e.Y);
//    		logWindow.writeLine(" e.X="+e.X+" e.Y="+e.Y+" boardX="+boardX+" boardY="+boardY);
			
			// Bounds check
			if (boardX >= boardSize || boardX < 0 || boardY >= boardSize || boardY < 0)
				return;
			
			// Check we're supposed to react to a board click
			if(mode!=Mode.GameInProgress && mode!=Mode.NavigatingSGF && mode!=Mode.OpeningSGF)
				return;
			
			// Left button plays a move
			if (e.Button == MouseButtons.Left) {
				if(mode==Mode.GameInProgress)
					HumanPlayMove(boardX, boardY);
			}
			// Rght button updates NN score box
			else if(e.Button == MouseButtons.Right) {
				// Format the float, argument 0, with 4 decimal places, using "F" float format
				nnScoreTextBox.Text = String.Format("{0:F4}", nnScores[boardY * boardSize + boardX]);
			}
		}
		
		void BoardPictureBoxMouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			mouseDownOnBoard = true;
		}

		void BoardPictureBoxLayout(object sender, System.Windows.Forms.LayoutEventArgs e)
		{
			// Repaint the board for the new client size
			// Guard against a minimised window by checking for size==0
			if(!boardPictureBox.Size.IsEmpty)
				PaintBoard();
		}		

		#region Button clicking functions
		
		void ScoreBlackMovesButtonClick(object sender, System.EventArgs e)
		{
			if (MelkorWrapper.DllGetScoredPoints(nnScores, boardSize, MelkorWrapper.BLACK) == 1) {
				drawNNScores = true;
				RedrawBoard();
			}
		}
		
		void ScoreWhiteMovesButtonClick(object sender, System.EventArgs e)
		{
			if (MelkorWrapper.DllGetScoredPoints(nnScores, boardSize, MelkorWrapper.WHITE) == 1) {
				drawNNScores = true;
				RedrawBoard();
			}
		}
		
		void ClearScoredMovesButtonClick(object sender, System.EventArgs e)
		{
			drawNNScores = false;
			RedrawBoard();
		}
		
		void PassButtonClick(object sender, System.EventArgs e)
		{
				HumanPlayMove(-1, -1);
				scoreButtonClicked = false;
		}
		
		void UndoButtonClick(object sender, System.EventArgs e)
		{
			// Only undo if one or more players are human
			if (blackPlayer == COMPUTER && whitePlayer == COMPUTER)
				return;
			
			// Do one undo
			if (MelkorWrapper.DllUndo() == 0) {
				MessageBox.Show("Undo not allowed.");
				return;
			}
			if(nextToPlay == MelkorWrapper.BLACK)
				changeTurn(MelkorWrapper.WHITE);
			else
				changeTurn(MelkorWrapper.BLACK);
			
			// If playing against a computer, undo twice
			if(blackPlayer == COMPUTER || whitePlayer == COMPUTER) {
				MelkorWrapper.DllUndo();
				if(nextToPlay == MelkorWrapper.BLACK)
					changeTurn(MelkorWrapper.WHITE);
				else
					changeTurn(MelkorWrapper.BLACK);
			}
			scoreButtonClicked = false;
			RedrawBoard();
		}
		
		void ForwardButtonClick(object sender, System.EventArgs e)
		{
			MelkorWrapper.DllNextSGF(0);
			scoreButtonClicked = false;
			resetBoardStatusArrays();
			RedrawBoard();
		}
		
		void ForwardStepButtonClick(object sender, System.EventArgs e)
		{
			int step;
			
			try {
				step = Int32.Parse(sgfStepTextBox.Text);

				if(step<0) {
					MessageBox.Show("Step value must be a positive integer.");
					return;
				}
				MelkorWrapper.DllNextSGF(step);
				scoreButtonClicked = false;
				resetBoardStatusArrays();
				RedrawBoard();
			}
			catch (FormatException ex) {
				MessageBox.Show("Step value must be a positive integer: "+ex.ToString());
			}
			catch (OverflowException ex) {
				MessageBox.Show("Step value is too large: "+ex.ToString());
			}
		}
		
		void ForwardToEndButtonClick(object sender, System.EventArgs e)
		{
			// Unless we know the size of the SGF just step a very large number
			MelkorWrapper.DllNextSGF(32000);
			scoreButtonClicked = false;
			resetBoardStatusArrays();
			RedrawBoard();
		}
		
		void BackButtonClick(object sender, System.EventArgs e)
		{
			MelkorWrapper.DllBackSGF(0);
			scoreButtonClicked = false;
			resetBoardStatusArrays();
			RedrawBoard();
		}
		
		void BackStepButtonClick(object sender, System.EventArgs e)
		{
			int step;
			
			try {
				step = Int32.Parse(sgfStepTextBox.Text);

				if(step<0) {
					MessageBox.Show("Step value must be a positive integer.");
					return;
				}
				MelkorWrapper.DllBackSGF(step);
				scoreButtonClicked = false;
				resetBoardStatusArrays();
				RedrawBoard();
			}
			catch (FormatException ex) {
				MessageBox.Show("Step value must be a positive integer: "+ex.ToString());
			}
			catch (OverflowException ex) {
				MessageBox.Show("Step value is too large: "+ex.ToString());
			}
		}
		
		void BackToStartButtonClick(object sender, System.EventArgs e)
		{
			// Unless we know the size of the SGF just step a very large number
			MelkorWrapper.DllBackSGF(32000);
			scoreButtonClicked = false;
			resetBoardStatusArrays();
			RedrawBoard();
		}
		
		void ScoreButtonClick(object sender, System.EventArgs e)
		{
			if(!scoreButtonClicked) {
				scoreButtonClicked = true;
				scoreBoard(false);
			}
			else {
				scoreButtonClicked = false;
				ResetDeadStones();
				ResetTerritoryMarkers();
				RedrawBoard();
			}
		}

		#endregion
		
		#region Menu item clicking functions
		void NewMenuItemClick(object sender, System.EventArgs e)
		{
			DialogResult res = newDialog.ShowDialog();
			
			if(res==DialogResult.OK) {
				if(newDialog.size9x9RadioButton.Checked)
					ResetBoard(9);
				else if(newDialog.size19x19RadioButton.Checked)
					ResetBoard(19);
				if(newDialog.humanBlackRadioButton.Checked)
					blackPlayer = HUMAN;
				else {
					blackPlayer = COMPUTER;					
					MelkorWrapper.DllSetBlackName(computerName);  // Set SGF name
				}
				if(newDialog.humanWhiteRadioButton.Checked)
					whitePlayer = HUMAN;
				else {
					whitePlayer = COMPUTER;
					MelkorWrapper.DllSetWhiteName(computerName);  // Set SGF name
				}
				
				// Set up the game, handicap etc...			
				MelkorWrapper.DllSetKomi(Convert.ToSingle(newDialog.komiNumericUpDown.Value));
				MelkorWrapper.DllSetHandicap(Convert.ToInt32(newDialog.handicapNumericUpDown.Value));
				
				// If theres a handicap then WHITE will play first
				if(newDialog.handicapNumericUpDown.Value>0)
					changeTurn(MelkorWrapper.WHITE);
				else
					changeTurn(MelkorWrapper.BLACK);
				
				RedrawBoard();
				
				// Start the timers if there is a game time limit - in seconds don't forget to 
				// convert from the mins in the new dialog
				initialByoyomiTime = Convert.ToInt32(newDialog.byoyomiTimeNumericUpDown.Value)*60;
				initialByoyomiStones = Convert.ToInt32(newDialog.byoyomiStonesNumericUpDown.Value);
				MelkorWrapper.DllSetTimeSettings(Convert.ToInt32(newDialog.mainTimeNumericUpDown.Value)*60,
				                                 initialByoyomiTime, initialByoyomiStones);
				if(newDialog.useTimeLimitCheckBox.Checked) {
					timedGame = true;
					MelkorWrapper.DllPlayTimedGame(1);
				}
				else {
					timedGame = false;
					MelkorWrapper.DllPlayTimedGame(0);
				}
				
				currentMainTimeBlack = Convert.ToInt32(newDialog.mainTimeNumericUpDown.Value)*60;
				currentMainTimeWhite = Convert.ToInt32(newDialog.mainTimeNumericUpDown.Value)*60;
				currentByoyomiTimeBlack = initialByoyomiTime;
				currentByoyomiTimeWhite = initialByoyomiTime;
				currentByoyomiStonesBlack = Convert.ToInt32(newDialog.byoyomiStonesNumericUpDown.Value);
				currentByoyomiStonesWhite = Convert.ToInt32(newDialog.byoyomiStonesNumericUpDown.Value);

				// Update the GUI
				updateGUITimeDisplay();
				resultTextBox.Text = "";
				
				// Update the state flags
				setMode(Mode.GameInProgress);
				
				// Generate a computer move (implicit timer start) or if it's the
				// human to play - just start the timer and wait
				if(nextToPlay==MelkorWrapper.BLACK && blackPlayer==COMPUTER ||
				   nextToPlay==MelkorWrapper.WHITE && whitePlayer==COMPUTER)
					ComputerGenerateMove();
				else if(timedGame)
					oneSecondTimer.Start();
			}
		}
		
		void OpenMenuItemClick(object sender, System.EventArgs e)
		{
			// Use a file chooser dialog to select an sgf file
			OpenFileDialog ofd = new OpenFileDialog();
			ofd.Filter = "sgf files (*.sgf)|*.sgf|mgt files (*.mgt)|*.mgt|All files (*.*)|*.*";
			ofd.FilterIndex = 0;
			ofd.RestoreDirectory = true;
			
			if(ofd.ShowDialog() == DialogResult.OK) {
        		if(ofd.FileName == "")
        			return;
    		}
			
			MelkorWrapper.DllLoadSGF(ofd.FileName);
			
			setMode(Mode.OpeningSGF);
			ResetBoardDontUpdateDLL(MelkorWrapper.DllGetBoardSize());
			RedrawBoard();
		}
		
		void SaveAsMenuItemClick(object sender, System.EventArgs e)
		{
     		SaveFileDialog saveFileDialog1 = new SaveFileDialog();
 
     		saveFileDialog1.Filter = "sgf files (*.sgf)|*.sgf|All files (*.*)|*.*"  ;
     		saveFileDialog1.FilterIndex = 1;
     		saveFileDialog1.RestoreDirectory = true ;
 
		    if(saveFileDialog1.ShowDialog() == DialogResult.OK)
     			MelkorWrapper.DllSaveSGF(saveFileDialog1.FileName);
		    
		    /// TODO Store the filename and update the window title
		    SGFFilename = saveFileDialog1.FileName;
     	}

		void ExitMenuItemClick(object sender, System.EventArgs e)
		{
			Close();
		}
		
		void LogWindowMenuItemClick(object sender, System.EventArgs e)
		{
			if(logWindowMenuItem.Checked) {
				logWindowMenuItem.Checked = false;
				logWindow.Hide();
			}
			else {
				logWindowMenuItem.Checked = true;
				logWindow.Show();
			}
		}
		
		void AboutMenuItemClick(object sender, System.EventArgs e)
		{
			StringBuilder aboutString = new StringBuilder();
			aboutString.Append(computerName);
			// version string adds a null character to the end of the string
			// remove this before adding more information
			/// TODO This doesn't work!
			aboutString.Remove(aboutString.Length-1, 1);
			aboutString.Append("\n\nBy Julian Churchill - Copyright 2005");
			MessageBox.Show(aboutString.ToString(), "About Melkor");
		}
	
		void DisplaySearchSettings(object sender, System.EventArgs e)
		{
			DialogResult res = searchSettings.ShowDialog();
			
			if(res==DialogResult.OK) {
				// Update Melkor with the new settings
				MelkorWrapper.DllSetStatusDepth(Convert.ToInt32(searchSettings.getStatusDepthNumericUpDown().Value));
				MelkorWrapper.DllSetUseDynamicSearchDepth(searchSettings.getUseDynamicSearchDepthCheckBox().Checked);
				MelkorWrapper.DllSetTimePerMove(Convert.ToInt32(searchSettings.getMoveTimeLimitNumericUpDown().Value));
			}
		}
		
		#endregion
		
		#region GUI update functions

		// Update our captured stone counts from Melkor
		void updateCapturedStones()
		{
			// blackCaptured box tells how many black has captured...
			blackCapturedTextBox.Text = Convert.ToString(MelkorWrapper.DllGetCapturedWhiteStones());
			whiteCapturedTextBox.Text = Convert.ToString(MelkorWrapper.DllGetCapturedBlackStones());
		}
		
		void updateGUITimeDisplay()
		{
			updateBlackMainTime();
			updateBlackByoyomi();
			updateWhiteMainTime();
			updateWhiteByoyomi();
		}
		
		void updateBlackMainTime() 
		{
			int bmMinutes = currentMainTimeBlack/60;
			int bmSeconds = currentMainTimeBlack - bmMinutes*60;
			blackTimeTextBox.Text = bmMinutes+":";
			if(bmSeconds<10)
				blackTimeTextBox.AppendText("0");
			blackTimeTextBox.AppendText(Convert.ToString(bmSeconds));
		}
		
		void updateBlackByoyomi()
		{
			int bbyMinutes = currentByoyomiTimeBlack/60;
			int bbySeconds = currentByoyomiTimeBlack - bbyMinutes*60;
			blackByoyomiTextBox.Text = bbyMinutes+":";
			if(bbySeconds<10)
				blackByoyomiTextBox.AppendText("0");
			blackByoyomiTextBox.AppendText(Convert.ToString(bbySeconds));
			blackByoyomiTextBox.AppendText("/"+currentByoyomiStonesBlack);
		}
		
		void updateWhiteMainTime()
		{
			int wmMinutes = currentMainTimeWhite/60;
			int wmSeconds = currentMainTimeWhite - wmMinutes*60;
			whiteTimeTextBox.Text = wmMinutes+":";
			if(wmSeconds<10)
				whiteTimeTextBox.AppendText("0");
			whiteTimeTextBox.AppendText(Convert.ToString(wmSeconds));
		}
		
		void updateWhiteByoyomi()
		{
			int wbyMinutes = currentByoyomiTimeWhite/60;
			int wbySeconds = currentByoyomiTimeWhite - wbyMinutes*60;	
			whiteByoyomiTextBox.Text = wbyMinutes+":";
			if(wbySeconds<10)
				whiteByoyomiTextBox.AppendText("0");
			whiteByoyomiTextBox.AppendText(Convert.ToString(wbySeconds));
			whiteByoyomiTextBox.AppendText("/"+currentByoyomiStonesWhite);
		}
		
		void updateByoyomi()
		{
			if(nextToPlay==MelkorWrapper.BLACK && currentMainTimeBlack<=0) {
				currentByoyomiStonesBlack--;
				if(currentByoyomiStonesBlack<=0) {
					currentByoyomiStonesBlack = initialByoyomiStones;
					currentByoyomiTimeBlack = initialByoyomiTime;
				}
				updateBlackByoyomi();
			}
			else if(nextToPlay==MelkorWrapper.WHITE && currentMainTimeWhite<=0) {
				currentByoyomiStonesWhite--;
				if(currentByoyomiStonesWhite<=0) {
					currentByoyomiStonesWhite = initialByoyomiStones;
					currentByoyomiTimeWhite = initialByoyomiTime;
				}
				updateWhiteByoyomi();
			}
		}

		#endregion
		
		void OneSecondTimerTick(object sender, System.EventArgs e)
		{
			// Update the time for the current player
			if(nextToPlay==MelkorWrapper.BLACK) {
				if(currentMainTimeBlack>0) {
					currentMainTimeBlack--;
					updateBlackMainTime();
				}
				else if(currentByoyomiTimeBlack>0) {
					currentByoyomiTimeBlack--;
					updateBlackByoyomi();
				}
				else {
					oneSecondTimer.Stop();
					setMode(Mode.NavigatingSGF);
					resultString = "W+Time";
					MessageBox.Show("White wins on time!");
					endGame(true);
				}
			}
			else {
				if(currentMainTimeWhite>0) {
					currentMainTimeWhite--;
					updateWhiteMainTime();
				}
				else if(currentByoyomiTimeWhite>0) {
					currentByoyomiTimeWhite--;
					updateWhiteByoyomi();
				}
				else {
					oneSecondTimer.Stop();
					setMode(Mode.NavigatingSGF);
					resultString = "B+Time";
					MessageBox.Show("Black wins on time!");
					endGame(true);
				}
			}		
		}

		void setMode(Mode newMode)
		{
			// Set the GUI state according to the mode specified
			if(newMode==Mode.OpeningSGF || newMode==Mode.NavigatingSGF) {
				if(newMode==Mode.OpeningSGF) {
					/// TODO Get the result string from the SGF file
					resultString = "";
				}
				mode = Mode.NavigatingSGF;
				
				// Enable SGF navigation controls
				forwardButton.Enabled = true;
				forwardStepButton.Enabled = true;
				forwardToEndButton.Enabled = true;
				backButton.Enabled = true;
				backStepButton.Enabled = true;
				backToStartButton.Enabled = true;
				sgfStepTextBox.Enabled = true;
			
				// Disable pass and undo buttons
				passButton.Enabled = false;
				undoButton.Enabled = false;
				
				// Enable score button
				scoreButton.Enabled = true;
				scoreButtonClicked = false;
				
				// Menu items
				playerMenu.Enabled = true;
			}
			else if(newMode==Mode.GameInProgress) {
				mode = Mode.GameInProgress;
				
				// Disable SGF navigation controls
				forwardButton.Enabled = false;
				forwardStepButton.Enabled = false;
				forwardToEndButton.Enabled = false;
				backButton.Enabled = false;
				backStepButton.Enabled = false;
				backToStartButton.Enabled = false;
				sgfStepTextBox.Enabled = false;
			
				// Enable pass and undo buttons
				passButton.Enabled = true;
				undoButton.Enabled = true;

				// Enable score button
				scoreButton.Enabled = true;
				scoreButtonClicked = false;

				// Menu items
				playerMenu.Enabled = false;
			}
		}
		
		// Score the board and display a final score dialog and highlight territory also.
		void endGame(bool lostOnTime)
		{
			setMode(Mode.NavigatingSGF);
			scoreBoard(lostOnTime);			
			// Display a final score dialog
			MelkorWrapper.DllSetSGFResultString(resultString);
			logWindow.writeLine("Final score: "+resultString);
			MessageBox.Show("Final score: "+resultString);
		}

		void scoreBoard(bool lostOnTime) {
			// Set resultString
			if(!lostOnTime) {
				float score;
				if(mode==Mode.GameInProgress)
					score = MelkorWrapper.DllScoreBoard();
				else {
					bool scoreButtonState = scoreButton.Enabled;
					scoreButton.Enabled = false;  // Temporarily disable the score button
					score = MelkorWrapper.DllFinalScore();
					scoreButton.Enabled = scoreButtonState;
				}
				// Win for White...
				if(score<0) {
					score = -score;
					resultString = "W+"+Convert.ToString(score);
				}
				// Win for Black...
				else
					resultString = "B+"+Convert.ToString(score);
			}
			resultTextBox.Text = resultString;

			// Get dead stones
			int[] inBoard = new int[boardSize*boardSize];
			resetBoardStatusArrays();
			MelkorWrapper.DllGetDeadStones(inBoard, boardSize);
			for(int i=0;i<boardSize*boardSize;i++) {
				if(inBoard[i]!=0)
					drawFadedStones[i] = true;
			}
				
			// Mark territory for highlighting
			MelkorWrapper.DllGetTerritoryBlack(inBoard, boardSize);
			for(int i=0;i<boardSize*boardSize;i++) {
				if(inBoard[i]!=0)
					blackTerritory[i] = true;
			}
			MelkorWrapper.DllGetTerritoryWhite(inBoard, boardSize);
			for(int i=0;i<boardSize*boardSize;i++) {
				if(inBoard[i]!=0)
					whiteTerritory[i] = true;
			}

			RedrawBoard();
		}
		
		void MainFormClosing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			// Check if the user is certain they want to exit...
			if (MessageBox.Show("Are you sure you want to exit?", "Exiting...", 
			                    MessageBoxButtons.YesNo) == DialogResult.No) {
				// Cancel the closing event from closing the form
				e.Cancel = true;
				return;
			}
			
			// Stop logging DLL output to a file
			MelkorWrapper.DllLogStop();

			// Cleanup the MelkorDLL.dll if we are actually closing
			MelkorWrapper.cleanupDll();
		}
			
		void logWindowClosing(object source, System.ComponentModel.CancelEventArgs e) {
			logWindowMenuItem.Checked = false;
			logWindow.Hide();
		}				
		
	}
}
