/*
 * Created by SharpDevelop.
 * User: Administrator
 * Date: 15/12/2004
 * Time: 14:06
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */

using System;
using System.Drawing;
using System.Windows.Forms;

/// TODO Add komi and handicap

namespace MelkorNETGUI
{
	/// <summary>
	/// Description of Form1.
	/// </summary>
	public class NewDialog : System.Windows.Forms.Form
	{
		public System.Windows.Forms.NumericUpDown komiNumericUpDown;
		private System.Windows.Forms.Button cancelButton;
		private System.Windows.Forms.Label handicapLabel;
		public System.Windows.Forms.RadioButton humanWhiteRadioButton;
		private System.Windows.Forms.Button okButton;
		private System.Windows.Forms.GroupBox whiteGroupBox;
		private System.Windows.Forms.Label byoyyomiLabel;
		private System.Windows.Forms.Label komiLabel;
		private System.Windows.Forms.GroupBox sizeGroupBox;
		public System.Windows.Forms.RadioButton size19x19RadioButton;
		public System.Windows.Forms.RadioButton computerBlackRadioButton;
		public System.Windows.Forms.NumericUpDown byoyomiTimeNumericUpDown;
		public System.Windows.Forms.RadioButton humanBlackRadioButton;
		public System.Windows.Forms.RadioButton size9x9RadioButton;
		public System.Windows.Forms.NumericUpDown mainTimeNumericUpDown;
		public System.Windows.Forms.NumericUpDown handicapNumericUpDown;
		public System.Windows.Forms.NumericUpDown byoyomiStonesNumericUpDown;
		private System.Windows.Forms.GroupBox blackGroupBox;
		public System.Windows.Forms.RadioButton computerWhiteRadioButton;
		public System.Windows.Forms.CheckBox useTimeLimitCheckBox;
		private System.Windows.Forms.Label byoyomiStonesLabel;
		private System.Windows.Forms.Label mainTimeLabel;
		public NewDialog()
		{
			//
			// The InitializeComponent() call is required for Windows Forms designer support.
			//
			InitializeComponent();
		}
		
		#region Windows Forms Designer generated code
		/// <summary>
		/// This method is required for Windows Forms designer support.
		/// Do not change the method contents inside the source code editor. The Forms designer might
		/// not be able to load this method if it was changed manually.
		/// </summary>
		private void InitializeComponent() {
			this.mainTimeLabel = new System.Windows.Forms.Label();
			this.byoyomiStonesLabel = new System.Windows.Forms.Label();
			this.useTimeLimitCheckBox = new System.Windows.Forms.CheckBox();
			this.computerWhiteRadioButton = new System.Windows.Forms.RadioButton();
			this.blackGroupBox = new System.Windows.Forms.GroupBox();
			this.byoyomiStonesNumericUpDown = new System.Windows.Forms.NumericUpDown();
			this.handicapNumericUpDown = new System.Windows.Forms.NumericUpDown();
			this.mainTimeNumericUpDown = new System.Windows.Forms.NumericUpDown();
			this.size9x9RadioButton = new System.Windows.Forms.RadioButton();
			this.humanBlackRadioButton = new System.Windows.Forms.RadioButton();
			this.byoyomiTimeNumericUpDown = new System.Windows.Forms.NumericUpDown();
			this.computerBlackRadioButton = new System.Windows.Forms.RadioButton();
			this.size19x19RadioButton = new System.Windows.Forms.RadioButton();
			this.sizeGroupBox = new System.Windows.Forms.GroupBox();
			this.komiLabel = new System.Windows.Forms.Label();
			this.byoyyomiLabel = new System.Windows.Forms.Label();
			this.whiteGroupBox = new System.Windows.Forms.GroupBox();
			this.okButton = new System.Windows.Forms.Button();
			this.humanWhiteRadioButton = new System.Windows.Forms.RadioButton();
			this.handicapLabel = new System.Windows.Forms.Label();
			this.cancelButton = new System.Windows.Forms.Button();
			this.komiNumericUpDown = new System.Windows.Forms.NumericUpDown();
			this.blackGroupBox.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.byoyomiStonesNumericUpDown)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.handicapNumericUpDown)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.mainTimeNumericUpDown)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.byoyomiTimeNumericUpDown)).BeginInit();
			this.sizeGroupBox.SuspendLayout();
			this.whiteGroupBox.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.komiNumericUpDown)).BeginInit();
			this.SuspendLayout();
			// 
			// mainTimeLabel
			// 
			this.mainTimeLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.mainTimeLabel.Location = new System.Drawing.Point(152, 192);
			this.mainTimeLabel.Name = "mainTimeLabel";
			this.mainTimeLabel.Size = new System.Drawing.Size(96, 23);
			this.mainTimeLabel.TabIndex = 10;
			this.mainTimeLabel.Text = "Main Time (Mins)";
			// 
			// byoyomiStonesLabel
			// 
			this.byoyomiStonesLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.byoyomiStonesLabel.Location = new System.Drawing.Point(152, 256);
			this.byoyomiStonesLabel.Name = "byoyomiStonesLabel";
			this.byoyomiStonesLabel.Size = new System.Drawing.Size(96, 23);
			this.byoyomiStonesLabel.TabIndex = 14;
			this.byoyomiStonesLabel.Text = "Byo-Yomi Stones";
			// 
			// useTimeLimitCheckBox
			// 
			this.useTimeLimitCheckBox.Checked = true;
			this.useTimeLimitCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
			this.useTimeLimitCheckBox.Location = new System.Drawing.Point(152, 160);
			this.useTimeLimitCheckBox.Name = "useTimeLimitCheckBox";
			this.useTimeLimitCheckBox.TabIndex = 15;
			this.useTimeLimitCheckBox.Text = "Use Time Limit";
			this.useTimeLimitCheckBox.CheckedChanged += new System.EventHandler(this.UseTimeLimitCheckBoxCheckedChanged);
			// 
			// computerWhiteRadioButton
			// 
			this.computerWhiteRadioButton.Checked = true;
			this.computerWhiteRadioButton.Location = new System.Drawing.Point(8, 32);
			this.computerWhiteRadioButton.Name = "computerWhiteRadioButton";
			this.computerWhiteRadioButton.Size = new System.Drawing.Size(72, 24);
			this.computerWhiteRadioButton.TabIndex = 0;
			this.computerWhiteRadioButton.TabStop = true;
			this.computerWhiteRadioButton.Text = "Computer";
			this.computerWhiteRadioButton.CheckedChanged += new System.EventHandler(this.ComputerWhiteRadioButtonCheckedChanged);
			// 
			// blackGroupBox
			// 
			this.blackGroupBox.Controls.Add(this.humanBlackRadioButton);
			this.blackGroupBox.Controls.Add(this.computerBlackRadioButton);
			this.blackGroupBox.Location = new System.Drawing.Point(24, 40);
			this.blackGroupBox.Name = "blackGroupBox";
			this.blackGroupBox.Size = new System.Drawing.Size(88, 104);
			this.blackGroupBox.TabIndex = 2;
			this.blackGroupBox.TabStop = false;
			this.blackGroupBox.Text = "Black";
			// 
			// byoyomiStonesNumericUpDown
			// 
			this.byoyomiStonesNumericUpDown.Location = new System.Drawing.Point(264, 256);
			this.byoyomiStonesNumericUpDown.Maximum = new System.Decimal(new int[] {
						1000,
						0,
						0,
						0});
			this.byoyomiStonesNumericUpDown.Name = "byoyomiStonesNumericUpDown";
			this.byoyomiStonesNumericUpDown.Size = new System.Drawing.Size(40, 20);
			this.byoyomiStonesNumericUpDown.TabIndex = 13;
			this.byoyomiStonesNumericUpDown.Value = new System.Decimal(new int[] {
						25,
						0,
						0,
						0});
			// 
			// handicapNumericUpDown
			// 
			this.handicapNumericUpDown.Location = new System.Drawing.Point(88, 192);
			this.handicapNumericUpDown.Maximum = new System.Decimal(new int[] {
						9,
						0,
						0,
						0});
			this.handicapNumericUpDown.Name = "handicapNumericUpDown";
			this.handicapNumericUpDown.Size = new System.Drawing.Size(40, 20);
			this.handicapNumericUpDown.TabIndex = 6;
			// 
			// mainTimeNumericUpDown
			// 
			this.mainTimeNumericUpDown.Location = new System.Drawing.Point(264, 192);
			this.mainTimeNumericUpDown.Maximum = new System.Decimal(new int[] {
						1000,
						0,
						0,
						0});
			this.mainTimeNumericUpDown.Name = "mainTimeNumericUpDown";
			this.mainTimeNumericUpDown.Size = new System.Drawing.Size(40, 20);
			this.mainTimeNumericUpDown.TabIndex = 9;
			this.mainTimeNumericUpDown.Value = new System.Decimal(new int[] {
						20,
						0,
						0,
						0});
			// 
			// size9x9RadioButton
			// 
			this.size9x9RadioButton.Checked = true;
			this.size9x9RadioButton.Location = new System.Drawing.Point(8, 32);
			this.size9x9RadioButton.Name = "size9x9RadioButton";
			this.size9x9RadioButton.Size = new System.Drawing.Size(72, 24);
			this.size9x9RadioButton.TabIndex = 0;
			this.size9x9RadioButton.TabStop = true;
			this.size9x9RadioButton.Text = "9x9";
			this.size9x9RadioButton.CheckedChanged += new System.EventHandler(this.ComputerWhiteRadioButtonCheckedChanged);
			// 
			// humanBlackRadioButton
			// 
			this.humanBlackRadioButton.Checked = true;
			this.humanBlackRadioButton.Location = new System.Drawing.Point(8, 64);
			this.humanBlackRadioButton.Name = "humanBlackRadioButton";
			this.humanBlackRadioButton.Size = new System.Drawing.Size(64, 24);
			this.humanBlackRadioButton.TabIndex = 1;
			this.humanBlackRadioButton.TabStop = true;
			this.humanBlackRadioButton.Text = "Human";
			// 
			// byoyomiTimeNumericUpDown
			// 
			this.byoyomiTimeNumericUpDown.Location = new System.Drawing.Point(264, 224);
			this.byoyomiTimeNumericUpDown.Maximum = new System.Decimal(new int[] {
						1000,
						0,
						0,
						0});
			this.byoyomiTimeNumericUpDown.Name = "byoyomiTimeNumericUpDown";
			this.byoyomiTimeNumericUpDown.Size = new System.Drawing.Size(40, 20);
			this.byoyomiTimeNumericUpDown.TabIndex = 11;
			this.byoyomiTimeNumericUpDown.Value = new System.Decimal(new int[] {
						10,
						0,
						0,
						0});
			// 
			// computerBlackRadioButton
			// 
			this.computerBlackRadioButton.Location = new System.Drawing.Point(8, 32);
			this.computerBlackRadioButton.Name = "computerBlackRadioButton";
			this.computerBlackRadioButton.Size = new System.Drawing.Size(72, 24);
			this.computerBlackRadioButton.TabIndex = 0;
			this.computerBlackRadioButton.Text = "Computer";
			this.computerBlackRadioButton.CheckedChanged += new System.EventHandler(this.ComputerBlackRadioButtonCheckedChanged);
			// 
			// size19x19RadioButton
			// 
			this.size19x19RadioButton.Location = new System.Drawing.Point(8, 64);
			this.size19x19RadioButton.Name = "size19x19RadioButton";
			this.size19x19RadioButton.Size = new System.Drawing.Size(64, 24);
			this.size19x19RadioButton.TabIndex = 1;
			this.size19x19RadioButton.Text = "19x19";
			// 
			// sizeGroupBox
			// 
			this.sizeGroupBox.Controls.Add(this.size19x19RadioButton);
			this.sizeGroupBox.Controls.Add(this.size9x9RadioButton);
			this.sizeGroupBox.Location = new System.Drawing.Point(232, 40);
			this.sizeGroupBox.Name = "sizeGroupBox";
			this.sizeGroupBox.Size = new System.Drawing.Size(88, 104);
			this.sizeGroupBox.TabIndex = 4;
			this.sizeGroupBox.TabStop = false;
			this.sizeGroupBox.Text = "Size";
			// 
			// komiLabel
			// 
			this.komiLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.komiLabel.Location = new System.Drawing.Point(24, 160);
			this.komiLabel.Name = "komiLabel";
			this.komiLabel.Size = new System.Drawing.Size(56, 23);
			this.komiLabel.TabIndex = 7;
			this.komiLabel.Text = "Komi";
			// 
			// byoyyomiLabel
			// 
			this.byoyyomiLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.byoyyomiLabel.Location = new System.Drawing.Point(152, 224);
			this.byoyyomiLabel.Name = "byoyyomiLabel";
			this.byoyyomiLabel.Size = new System.Drawing.Size(96, 23);
			this.byoyyomiLabel.TabIndex = 12;
			this.byoyyomiLabel.Text = "Byo-Yomi (Mins)";
			// 
			// whiteGroupBox
			// 
			this.whiteGroupBox.Controls.Add(this.humanWhiteRadioButton);
			this.whiteGroupBox.Controls.Add(this.computerWhiteRadioButton);
			this.whiteGroupBox.Location = new System.Drawing.Point(128, 40);
			this.whiteGroupBox.Name = "whiteGroupBox";
			this.whiteGroupBox.Size = new System.Drawing.Size(88, 104);
			this.whiteGroupBox.TabIndex = 3;
			this.whiteGroupBox.TabStop = false;
			this.whiteGroupBox.Text = "White";
			// 
			// okButton
			// 
			this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.okButton.Location = new System.Drawing.Point(88, 296);
			this.okButton.Name = "okButton";
			this.okButton.TabIndex = 0;
			this.okButton.Text = "OK";
			this.okButton.Click += new System.EventHandler(this.OkButtonClick);
			// 
			// humanWhiteRadioButton
			// 
			this.humanWhiteRadioButton.Location = new System.Drawing.Point(8, 64);
			this.humanWhiteRadioButton.Name = "humanWhiteRadioButton";
			this.humanWhiteRadioButton.Size = new System.Drawing.Size(64, 24);
			this.humanWhiteRadioButton.TabIndex = 1;
			this.humanWhiteRadioButton.Text = "Human";
			// 
			// handicapLabel
			// 
			this.handicapLabel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.handicapLabel.Location = new System.Drawing.Point(24, 192);
			this.handicapLabel.Name = "handicapLabel";
			this.handicapLabel.Size = new System.Drawing.Size(56, 23);
			this.handicapLabel.TabIndex = 8;
			this.handicapLabel.Text = "Handicap";
			// 
			// cancelButton
			// 
			this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cancelButton.Location = new System.Drawing.Point(184, 296);
			this.cancelButton.Name = "cancelButton";
			this.cancelButton.TabIndex = 1;
			this.cancelButton.Text = "Cancel";
			// 
			// komiNumericUpDown
			// 
			this.komiNumericUpDown.DecimalPlaces = 1;
			this.komiNumericUpDown.Increment = new System.Decimal(new int[] {
						5,
						0,
						0,
						65536});
			this.komiNumericUpDown.Location = new System.Drawing.Point(88, 160);
			this.komiNumericUpDown.Name = "komiNumericUpDown";
			this.komiNumericUpDown.Size = new System.Drawing.Size(40, 20);
			this.komiNumericUpDown.TabIndex = 5;
			this.komiNumericUpDown.Value = new System.Decimal(new int[] {
						55,
						0,
						0,
						65536});
			// 
			// NewDialog
			// 
			this.AcceptButton = this.okButton;
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.CancelButton = this.cancelButton;
			this.ClientSize = new System.Drawing.Size(328, 335);
			this.ControlBox = false;
			this.Controls.Add(this.useTimeLimitCheckBox);
			this.Controls.Add(this.byoyomiStonesLabel);
			this.Controls.Add(this.byoyomiStonesNumericUpDown);
			this.Controls.Add(this.byoyyomiLabel);
			this.Controls.Add(this.byoyomiTimeNumericUpDown);
			this.Controls.Add(this.mainTimeLabel);
			this.Controls.Add(this.mainTimeNumericUpDown);
			this.Controls.Add(this.handicapLabel);
			this.Controls.Add(this.komiLabel);
			this.Controls.Add(this.handicapNumericUpDown);
			this.Controls.Add(this.komiNumericUpDown);
			this.Controls.Add(this.sizeGroupBox);
			this.Controls.Add(this.whiteGroupBox);
			this.Controls.Add(this.blackGroupBox);
			this.Controls.Add(this.cancelButton);
			this.Controls.Add(this.okButton);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "NewDialog";
			this.ShowInTaskbar = false;
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			this.Text = "New Game";
			this.blackGroupBox.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.byoyomiStonesNumericUpDown)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.handicapNumericUpDown)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.mainTimeNumericUpDown)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.byoyomiTimeNumericUpDown)).EndInit();
			this.sizeGroupBox.ResumeLayout(false);
			this.whiteGroupBox.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.komiNumericUpDown)).EndInit();
			this.ResumeLayout(false);
		}
		#endregion
		void ComputerBlackRadioButtonCheckedChanged(object sender, System.EventArgs e)
		{
			if(computerBlackRadioButton.Checked && computerWhiteRadioButton.Checked) {
				MessageBox.Show("Currently cannot play 2 computer players...");
				computerBlackRadioButton.Checked = false;
				humanBlackRadioButton.Checked = true;
			}
		}
		
		void ComputerWhiteRadioButtonCheckedChanged(object sender, System.EventArgs e)
		{
			if(computerBlackRadioButton.Checked && computerWhiteRadioButton.Checked) {
				MessageBox.Show("Currently cannot play 2 computer players...");
				computerWhiteRadioButton.Checked = false;
				humanWhiteRadioButton.Checked = true;
			}
		}
		void UseTimeLimitCheckBoxCheckedChanged(object sender, System.EventArgs e)
		{
			if(useTimeLimitCheckBox.Checked) {
				mainTimeNumericUpDown.Enabled = true;
				byoyomiTimeNumericUpDown.Enabled = true;
				byoyomiStonesNumericUpDown.Enabled = true;
			}
			else {
				mainTimeNumericUpDown.Enabled = false;
				byoyomiTimeNumericUpDown.Enabled = false;
				byoyomiStonesNumericUpDown.Enabled = false;				
			}
		}
		
		void OkButtonClick(object sender, System.EventArgs e)
		{
			// Validate the return values in the dialog
			if(useTimeLimitCheckBox.Checked) {
				if((mainTimeNumericUpDown.Value==0 && byoyomiTimeNumericUpDown.Value==0) ||
					   (mainTimeNumericUpDown.Value==0 && byoyomiStonesNumericUpDown.Value==0)) {
					MessageBox.Show("Invalid time settings!");
					/// TODO Don't quit the dialog
//					e. = true;
					return;
				}
			}
		}
		
	}
}
